//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008-2009, Image Engine Design Inc. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of Image Engine Design nor the names of any
//       other contributors to this software may be used to endorse or
//       promote products derived from this software without specific prior
//       written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////

#ifndef IE_CORE_MESHPRIMITIVEEVALUATOR_H
#define IE_CORE_MESHPRIMITIVEEVALUATOR_H

#include <vector>

#include "IECore/PrimitiveEvaluator.h"
#include "IECore/MeshPrimitive.h"
#include "IECore/BoundedKDTree.h"

namespace IECore
{

/// An implementation of PrimitiveEvaluator to allow spatial queries to be performed on MeshPrimitive instances
class MeshPrimitiveEvaluator : public PrimitiveEvaluator
{
	public:

		typedef MeshPrimitive PrimitiveType;

		IE_CORE_DECLARERUNTIMETYPED( MeshPrimitiveEvaluator, PrimitiveEvaluator );

		class Result : public PrimitiveEvaluator::Result
		{
			friend class MeshPrimitiveEvaluator;

			public:

				IE_CORE_DECLAREMEMBERPTR( Result );

				Result();

				Imath::V3f point() const;
				Imath::V3f normal() const;
				Imath::V2f uv() const;
				Imath::V3f uTangent() const;
				Imath::V3f vTangent() const;

				Imath::V3f          vectorPrimVar( const PrimitiveVariable &pv ) const;
				float               floatPrimVar ( const PrimitiveVariable &pv ) const;
				int                 intPrimVar   ( const PrimitiveVariable &pv ) const;
				const std::string  &stringPrimVar( const PrimitiveVariable &pv ) const;
				Imath::Color3f      colorPrimVar ( const PrimitiveVariable &pv ) const;
				half                halfPrimVar  ( const PrimitiveVariable &pv ) const;

				unsigned int        triangleIndex() const;
				const Imath::V3f   &barycentricCoordinates() const;
				const Imath::V3i   &vertexIds() const;

			protected:

				Imath::V3i m_vertexIds;
				Imath::V3f m_bary;
				Imath::V3f m_p;
				Imath::V3f m_n;
				Imath::V2f m_uv;
				unsigned int m_triangleIdx;

				template<typename T>
				T getPrimVar( const PrimitiveVariable &pv ) const;


		};
		IE_CORE_DECLAREPTR( Result );

		static PrimitiveEvaluatorPtr create( ConstPrimitivePtr primitive );

		MeshPrimitiveEvaluator( ConstMeshPrimitivePtr mesh );

		virtual ~MeshPrimitiveEvaluator();

		virtual ConstPrimitivePtr primitive() const;

		virtual PrimitiveEvaluator::ResultPtr createResult() const;

		virtual void validateResult( const PrimitiveEvaluator::ResultPtr &result ) const;

		virtual bool closestPoint( const Imath::V3f &p, const PrimitiveEvaluator::ResultPtr &result ) const;

		virtual bool pointAtUV( const Imath::V2f &uv, const PrimitiveEvaluator::ResultPtr &result ) const;

		virtual bool intersectionPoint( const Imath::V3f &origin, const Imath::V3f &direction,
			const PrimitiveEvaluator::ResultPtr &result, float maxDistance = Imath::limits<float>::max() ) const;

		virtual int intersectionPoints( const Imath::V3f &origin, const Imath::V3f &direction,
			std::vector<PrimitiveEvaluator::ResultPtr> &results, float maxDistance = Imath::limits<float>::max() ) const;

		virtual bool signedDistance( const Imath::V3f &p, float &distance ) const;

		virtual float volume() const;

		virtual Imath::V3f centerOfGravity() const;

		virtual float surfaceArea() const;
		
		/// Returns a bounding box covering all the uv coordinates of the mesh.
		const Imath::Box2f uvBound() const;

	protected:

		ConstMeshPrimitivePtr m_mesh;
		ConstV3fVectorDataPtr m_verts;

		typedef Imath::Box3f TriangleBound;
		typedef std::vector<TriangleBound> TriangleBoundVector;
		TriangleBoundVector m_triangles;
		typedef BoundedKDTree<TriangleBoundVector::iterator> TriangleBoundTree;
		TriangleBoundTree *m_tree;

		typedef Imath::Box2f UVBound;
		typedef std::vector<UVBound> UVBoundVector;
		typedef BoundedKDTree<UVBoundVector::iterator> UVBoundTree;
		UVBoundVector m_uvTriangles;		
		UVBoundTree *m_uvTree;

		bool pointAtUVWalk( UVBoundTree::NodeIndex nodeIndex, const Imath::V2f &targetUV, const ResultPtr &result ) const;
		void closestPointWalk( TriangleBoundTree::NodeIndex nodeIndex, const Imath::V3f &p, float &closestDistanceSqrd, const ResultPtr &result ) const;
		bool intersectionPointWalk( TriangleBoundTree::NodeIndex nodeIndex, const Imath::Line3f &ray, float &maxDistSqrd, const ResultPtr &result, bool &hit ) const;
		void intersectionPointsWalk( TriangleBoundTree::NodeIndex nodeIndex, const Imath::Line3f &ray, float maxDistSqrd, std::vector<PrimitiveEvaluator::ResultPtr> &results ) const;

		void calculateMassProperties() const;
		void calculateAverageNormals() const;
		
		void triangleUVs( size_t triangleIndex, const Imath::V3i &vertexIds, Imath::V2f uv[3] ) const;
		PrimitiveVariable m_u;
		PrimitiveVariable m_v;

		mutable bool m_haveMassProperties;
		mutable float m_volume;
		mutable Imath::V3f m_centerOfGravity;
		mutable Imath::M33f m_inertia;

		mutable bool m_haveSurfaceArea;
		mutable float m_surfaceArea;

		mutable bool m_haveAverageNormals;
		typedef int VertexIndex;
		typedef int TriangleIndex;
		typedef std::pair<VertexIndex, VertexIndex> Edge;

		typedef std::map< Edge, Imath::V3f > EdgeAverageNormals;
		mutable EdgeAverageNormals m_edgeAverageNormals;

		mutable V3fVectorDataPtr m_vertexAngleWeightedNormals;

};

IE_CORE_DECLAREPTR( MeshPrimitiveEvaluator );

} // namespace IECore

#endif // IE_CORE_MESHPRIMITIVEEVALUATOR_H
