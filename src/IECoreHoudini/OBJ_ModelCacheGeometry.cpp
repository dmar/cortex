//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2013, Image Engine Design Inc. All rights reserved.
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

#include "IECoreHoudini/OBJ_ModelCacheGeometry.h"
#include "IECoreHoudini/SOP_ModelCacheSource.h"

using namespace IECore;
using namespace IECoreHoudini;

const char *OBJ_ModelCacheGeometry::typeName = "ieModelCacheGeometry";

OBJ_ModelCacheGeometry::OBJ_ModelCacheGeometry( OP_Network *net, const char *name, OP_Operator *op ) : OBJ_ModelCacheNode<OBJ_Geometry>( net, name, op )
{
}

OBJ_ModelCacheGeometry::~OBJ_ModelCacheGeometry()
{
}

OP_Node *OBJ_ModelCacheGeometry::create( OP_Network *net, const char *name, OP_Operator *op )
{
	return new OBJ_ModelCacheGeometry( net, name, op );
}

OP_TemplatePair *OBJ_ModelCacheGeometry::buildParameters()
{
	static OP_TemplatePair *templatePair = 0;
	if ( !templatePair )
	{
		templatePair = new OP_TemplatePair( *OBJ_ModelCacheNode<OBJ_Geometry>::buildParameters() );
	}
	
	return templatePair;
}

void OBJ_ModelCacheGeometry::buildHierarchy( const ModelCache *cache )
{
	doBuildGeometry( cache );
}

void OBJ_ModelCacheGeometry::doBuildGeometry( const ModelCache *cache )
{
	const char *name = cache->name().c_str();
	OP_Node *opNode = createNode( SOP_ModelCacheSource::typeName, name );
	SOP_ModelCacheSource *sop = reinterpret_cast<SOP_ModelCacheSource*>( opNode );
	
	sop->setFile( getFile() );
	sop->setPath( cache->path() );
	
	Space space = getSpace();
	UT_String shapes( name );
	SOP_ModelCacheSource::Space sopSpace = SOP_ModelCacheSource::Object;
	if ( space == World || space == Path )
	{
		shapes = "*";
		sopSpace = SOP_ModelCacheSource::Path;
	}
	
	sop->setSpace( sopSpace );
	sop->setString( shapes, CH_STRING_LITERAL, SOP_ModelCacheSource::pShapeFilter.getToken(), 0, 0 );
}