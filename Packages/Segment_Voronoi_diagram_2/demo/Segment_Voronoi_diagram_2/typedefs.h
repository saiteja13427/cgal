// Copyright (c) 2003,2004,2005  INRIA Sophia-Antipolis (France) and
// Notre Dame University (U.S.A.).  All rights reserved.
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
// Author(s)     : Menelaos Karavelas <mkaravel@cse.nd.edu>

#ifndef SVD_TYPEDEFS_H
#define SVD_TYPEDEFS_H

#include <CGAL/basic.h>

#define USE_FILTERED_TRAITS

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#ifdef CGAL_USE_CORE
#  include <CGAL/CORE_Expr.h>
#endif

#include <CGAL/Segment_Voronoi_diagram_2.h>
#include <CGAL/Segment_Voronoi_diagram_hierarchy_2.h>
#include <CGAL/Segment_Voronoi_diagram_traits_2.h>
#include <CGAL/Segment_Voronoi_diagram_filtered_traits_2.h>

#if defined(USE_FILTERED_TRAITS) || !defined(CGAL_USE_CORE)
struct Rep : public CGAL::Simple_cartesian<double> {};
#else
struct Rep : public CGAL::Simple_cartesian<CORE::Expr> {};
#endif

#ifdef USE_FILTERED_TRAITS
struct Gt
  : public CGAL::Segment_Voronoi_diagram_filtered_traits_2<Rep> {};
#else
struct Gt
  : public CGAL::Segment_Voronoi_diagram_traits_2<Rep,CGAL::Field_tag> {};
#endif

typedef Gt::Point_2            Point_2;
typedef Gt::Segment_2          Segment;
typedef CGAL::Polygon_2<Rep>   Polygon_2;
typedef Gt::Site_2             Site;

typedef CGAL::Tag_true         STag;

typedef CGAL::Segment_Voronoi_diagram_hierarchy_2<Gt,STag>       SVD_2;
//typedef CGAL::Segment_Voronoi_diagram_2<Gt>          SVD_2;

#endif  // SVD_TYPEDEFS_H
