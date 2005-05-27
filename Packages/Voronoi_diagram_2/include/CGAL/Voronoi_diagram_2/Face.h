// Copyright (c) 2005 University of Crete (Greece).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Menelaos Karavelas <mkaravel@tem.uoc.gr>

#ifndef CGAL_VORONOI_DIAGRAM_2_FACE_H
#define CGAL_VORONOI_DIAGRAM_2_FACE_H 1

#include <CGAL/Voronoi_diagram_adaptor_2/basic.h>
#include <cstdlib>
#include <CGAL/Triangulation_utils_2.h>

CGAL_BEGIN_NAMESPACE

CGAL_VORONOI_DIAGRAM_2_BEGIN_NAMESPACE

template<class VDA>
class Face
{
 private:
  typedef Face<VDA>                              Self;
  typedef typename VDA::Dual_edge_circulator     Dual_edge_circulator;
  typedef typename VDA::Dual_vertex_handle       Dual_vertex_handle;

  typedef Triangulation_cw_ccw_2                 CW_CCW_2;

 public:
  typedef typename VDA::Halfedge                 Halfedge;
  typedef typename VDA::Vertex                   Vertex;
  typedef typename VDA::Halfedge_handle          Halfedge_handle;
  typedef typename VDA::Vertex_handle            Vertex_handle;
  typedef typename VDA::Face_handle              Face_handle;
  typedef typename VDA::Ccb_halfedge_circulator  Ccb_halfedge_circulator;

  typedef typename VDA::Holes_iterator           Holes_iterator;
  typedef Holes_iterator                         Holes_const_iterator;

 private:
  static const Holes_iterator& null_iterator() {
    static Holes_iterator null_it;
    return null_it;
  }

 public:
  const Holes_iterator& holes_begin() { return null_iterator(); }
  const Holes_iterator& holes_end()   { return null_iterator(); }

  const Holes_const_iterator& holes_begin() const { return null_iterator(); }
  const Holes_const_iterator& holes_end() const   { return null_iterator(); }

 public:
  Face(const VDA* vda = NULL) : vda_(vda) {}
  Face(const VDA* vda, Dual_vertex_handle v) : vda_(vda), v_(v)
  {
    //    CGAL_precondition( !vda_->face_tester()(v_) );
  }

  bool is_unbounded() const {
    typedef typename VDA::Dual_graph::Vertex_circulator Dual_vertex_circulator;

    Dual_vertex_circulator vc = vda_->dual().incident_vertices(v_);
    Dual_vertex_circulator vc_start = vc;
    do {
      if ( vda_->dual().is_infinite(vc) ) { return true; }
      ++vc;
    } while ( vc != vc_start );
    return false;
  }

  Halfedge_handle halfedge_on_outer_ccb() const {
    return halfedge();
  }

  Halfedge_handle halfedge() const
  {
    // the edge circulator gives edges that have v_ as their target
    Dual_edge_circulator ec = vda_->dual().incident_edges(v_);
    Dual_edge_circulator ec_start = ec;

    // if I want to return also infinite edges replace the test in
    // the while loop by the following test (i.e., should omit the
    // testing for infinity):
    //           vda_->edge_tester()(vda_->dual(), ec)
    while ( vda_->edge_tester()(vda_->dual(), ec) ||
	    vda_->dual().is_infinite(ec) ) {
      ++ec;
      CGAL_assertion( ec != ec_start );
    }
    CGAL_assertion(ec->first->vertex( CW_CCW_2::cw(ec->second) ) == v_);

    int i_mirror = vda_->dual().tds().mirror_index(ec->first, ec->second);

#ifndef CGAL_NO_ASSERTIONS
    Halfedge h(vda_, ec->first->neighbor(ec->second), i_mirror);
    Face_handle f_this(*this);
    CGAL_assertion( h.face() == f_this );
#endif

    return
      Halfedge_handle( 
		      Halfedge(vda_, ec->first->neighbor(ec->second), i_mirror)
		      );
  }

  Ccb_halfedge_circulator outer_ccb() const {
    return Ccb_halfedge_circulator( *halfedge() );
  }

  bool is_halfedge_on_inner_ccb(const Halfedge_handle&) const {
    // MOST PROBABLY WHAT I NEED TO DO HERE IS TO RETURN false, SINCE
    // THERE ARE NO INNER CCBs.
    return false;
  }

  bool is_halfedge_on_outer_ccb(const Halfedge_handle& he) const {
    Ccb_halfedge_circulator hc_start = outer_ccb();
    Ccb_halfedge_circulator hc = hc_start;
    do {
      if ( he == *hc ) { return true; }
      hc++;
    } while ( hc != hc_start );
    return false;
  }

  bool operator==(const Self& other) const {
    if ( vda_ == NULL ) { return other.vda_ == NULL; }
    if ( other.vda_ == NULL ) { return vda_ == NULL; }
    return ( vda_ == other.vda_ && v_ == other.v_ );
  }

  bool operator!=(const Self& other) const {
    return !((*this) == other);
  }

  const Dual_vertex_handle& dual_vertex() const { return v_; }

  bool is_valid() const {
    if ( vda_ == NULL ) { return true; }

    bool valid = !vda_->face_tester()(vda_->dual(), v_);

    valid = valid && !vda_->edge_tester()( vda_->dual(),
					   halfedge()->dual_edge() );

    Ccb_halfedge_circulator hc = outer_ccb();
    Ccb_halfedge_circulator hc_start = hc;
    Face_handle f_this(*this);
    do {
      valid = valid && hc->face() == f_this;
      valid = valid && !vda_->edge_tester()( vda_->dual(),
					     hc->dual_edge() );
      hc++;
    } while ( hc != hc_start );

    return valid;
  }

private:
  const VDA* vda_;
  Dual_vertex_handle v_;
};

CGAL_VORONOI_DIAGRAM_2_END_NAMESPACE

CGAL_END_NAMESPACE


#endif // CGAL_VORONOI_DIAGRAM_2_FACE_H
