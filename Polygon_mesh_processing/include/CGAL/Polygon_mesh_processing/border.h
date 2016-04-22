// Copyright (c) 2015 GeometryFactory (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
//
// Author(s)     : Jane Tournois

#ifndef CGAL_POLYGON_MESH_PROCESSING_GET_BORDER_H
#define CGAL_POLYGON_MESH_PROCESSING_GET_BORDER_H

#include <boost/graph/graph_traits.hpp>
#include <boost/foreach.hpp>
#include <boost/type_traits/is_same.hpp>

#include <CGAL/Polygon_mesh_processing/internal/named_function_params.h>
#include <CGAL/Polygon_mesh_processing/internal/named_params_helper.h>

#include <set>

namespace CGAL{
namespace Polygon_mesh_processing {

  namespace internal
  {
    template<typename PM
           , typename FaceRange
           , typename HalfedgeOutputIterator>
    HalfedgeOutputIterator border_halfedges_impl(const FaceRange& faces
                                               , HalfedgeOutputIterator out
                                               , const PM& pmesh)
    {
      typedef typename boost::graph_traits<PM>::halfedge_descriptor halfedge_descriptor;
      typedef typename boost::graph_traits<PM>::face_descriptor     face_descriptor;

      //collect halfedges that appear only once
      // the bool is true if the halfedge stored is the one of the face,
      // false if it is its opposite
      std::map<halfedge_descriptor, bool> border;
      BOOST_FOREACH(face_descriptor f, faces)
      {
        BOOST_FOREACH(halfedge_descriptor h,
          halfedges_around_face(halfedge(f, pmesh), pmesh))
        {
          //halfedge_descriptor is model of `LessThanComparable`
          bool from_face = (h < opposite(h, pmesh));
          halfedge_descriptor he = from_face
            ? h
            : opposite(h, pmesh);
          if (border.find(he) != border.end())
            border.erase(he); //even number of appearances
          else
            border.insert(std::make_pair(he, from_face));//odd number of appearances
        }
      }
      //copy them in out
      typedef typename std::map<halfedge_descriptor, bool>::value_type HD_bool;
      BOOST_FOREACH(const HD_bool& hd, border)
      {
        if (hd.second)
          *out++ = hd.first;
        else
          *out++ = opposite(hd.first, pmesh);
      }
      return out;
    }

    template<typename PM
           , typename FaceRange
           , typename FaceIndexMap
           , typename HalfedgeOutputIterator>
    HalfedgeOutputIterator border_halfedges_impl(const FaceRange& faces
                                               , const FaceIndexMap& fmap
                                               , HalfedgeOutputIterator out
                                               , const PM& pmesh)
    {
      typedef typename boost::graph_traits<PM>::halfedge_descriptor halfedge_descriptor;
      typedef typename boost::graph_traits<PM>::face_descriptor     face_descriptor;

      std::vector<bool> present(num_faces(pmesh), false);
      BOOST_FOREACH(face_descriptor fd, faces)
        present[get(fmap, fd)] = true;

      BOOST_FOREACH(face_descriptor fd, faces)
        BOOST_FOREACH(halfedge_descriptor hd,
                      halfedges_around_face(halfedge(fd, pmesh), pmesh))
       {
         halfedge_descriptor opp=opposite(hd, pmesh);
         if (is_border(opp, pmesh) || !present[get(fmap,face(opp,pmesh))])
           *out++ = opp;
       }

      return out;
    }

    struct Dummy_PM
    {
    public:
      typedef bool vertex_property_type;
    };

  }//end namespace internal

  /*!
  \ingroup PkgPolygonMeshProcessing
  * collects the border of a surface patch
  * defined as a face range. The border is "seen from inside" the patch,
  * i.e. the collected halfedges are
  * the ones that belong to the input faces.
  *
  * @tparam PolygonMesh model of `HalfedgeGraph`. If `PolygonMesh
  *  `has an internal property map
  *  for `CGAL::face_index_t`, then it should be initialized
  * @tparam FaceRange range of
       `boost::graph_traits<PolygonMesh>::%face_descriptor`, model of `Range`.
        Its iterator type is `InputIterator`.
  * @tparam HalfedgeOutputIterator model of `OutputIterator`
     holding `boost::graph_traits<PolygonMesh>::%halfedge_descriptor`
     for patch border
  * @tparam NamedParameters a sequence of \ref namedparameters
  *
  * @param pmesh the polygon mesh to which `faces` belong
  * @param faces the range of faces defining the patch
  *              around which the border is collected
  * @param out the output iterator that collects halfedges that form the border
  *            of `faces`, seen from inside the surface patch
  * @param np optional sequence of \ref namedparameters among
     the ones listed below

  * \cgalNamedParamsBegin
      \cgalParamBegin{face_index_map} a property map containing the index of each face of `pmesh` \cgalParamEnd
    \cgalNamedParamsEnd
  *
  * @returns `out`
  */
  template<typename PolygonMesh
         , typename FaceRange
         , typename HalfedgeOutputIterator
         , typename NamedParameters>
  HalfedgeOutputIterator border_halfedges(const FaceRange& faces
                                  , const PolygonMesh& pmesh
                                  , HalfedgeOutputIterator out
                                  , const NamedParameters& np)
  {
    typedef PolygonMesh PM;
    typedef typename GetFaceIndexMap<PM, NamedParameters>::type           FIMap;
    typedef typename boost::property_map<typename internal::Dummy_PM,
                                              CGAL::face_index_t>::type   Unset_FIMap;

    if (boost::is_same<FIMap, Unset_FIMap>::value)
    {
      //face index map is not given in named parameters, nor as an internal property map
      return internal::border_halfedges_impl(faces, out, pmesh);
    }
    //face index map given as a named parameter, or as an internal property map
    FIMap fim = choose_param(get_param(np, face_index),
                             get(CGAL::face_index, pmesh));
    return internal::border_halfedges_impl(faces, fim, out, pmesh);
  }

  template<typename PolygonMesh
         , typename HalfedgeOutputIterator>
  HalfedgeOutputIterator border_halfedges(const PolygonMesh& pmesh
                                        , HalfedgeOutputIterator out)
  {
    typedef PolygonMesh PM;
    typedef typename boost::graph_traits<PM>::halfedge_descriptor halfedge_descriptor;
    BOOST_FOREACH(halfedge_descriptor hd, halfedges(pmesh))
      if (is_border(hd, pmesh))
        *out++ = hd;
    return out;
  }

  template<typename PolygonMesh
         , typename FaceRange
         , typename HalfedgeOutputIterator>
  HalfedgeOutputIterator border_halfedges(const FaceRange& faces
                                        , const PolygonMesh& pmesh
                                        , HalfedgeOutputIterator out)
  {
    return border_halfedges(faces, pmesh, out,
      CGAL::Polygon_mesh_processing::parameters::all_default());
  }

}
} // end of namespace CGAL::Polygon_mesh_processing


#endif //CGAL_POLYGON_MESH_PROCESSING_GET_BORDER_H
