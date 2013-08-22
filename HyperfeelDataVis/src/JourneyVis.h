//
//  JourneyVis.h
//  HyperfeelDataVis
//
//


#pragma once
#include "ofMain.h"
#include "Journey.h"

class JourneyVis {
public:
	JourneyVis()
	{
		needsUpdate = true;
		bWireframe = false;
		
		
		cout << color << endl;
	};
	~JourneyVis()
	{
		clear();
	};
	
	void clear(){
		vbo.clear();
	}
	
	void setup( Journey* j, ofColor c, float _twistCoefficient=0, float _minWidth=1.5, float _readingScale=1.)
	{
		journey = j;
		twistCoefficient = _twistCoefficient;
		halfMinWidth = _minWidth * .5;
		readingScale = _readingScale;
		
		color = c;
		
		update();
	}
	
	void update( bool forceUpdate = false)
	{
		if(forceUpdate || needsUpdate ){
			needsUpdate = false;
			
			
			if(journey != NULL){
				numIndices = 0;
				vector<ofVec3f> attentionVertices( journey->readings.size() );
				vector<ofVec3f> centralSpline( journey->readings.size() );
				vector<ofVec3f> meditationVertices( journey->readings.size() );
				
				vector<ofVec3f> vertices;
				vector <ofVec2f> texCoords;
				vector<ofIndexType> indices;
				
				ofQuaternion q;
				ofVec3f v0, v1, v2;
				float t, a, m;
				float totalWidth = halfMinWidth*2 + readingScale*2;
				float tcOffset = halfMinWidth / readingScale;
				for( int i=0 ; i<journey->readings.size(); i++){
					Reading r = journey->readings[i];
					t = journey->readings[i].getT();
					a = journey->readings[i].getAttention();
					m = journey->readings[i].getMeditation();
					
					q.makeRotate(t * twistCoefficient, 1, 0, 0 );
					
					//create our vertices
					v0.set( t * 1, a*readingScale + halfMinWidth, 0 );
					v1.set( t * 1, 0, -5 );
//					v1.set( t * 1, (a+m)*.5, 0 );
					v2.set( t * 1, -m*readingScale - halfMinWidth, 0 );
					
					v0 = q * v0;
					v1 = q * v1;
					v2 = q * v2;
					
					vertices.push_back( v0 );
					vertices.push_back( v1 );
					vertices.push_back( v2 );
					
					//scale the y tex coord so that the texture isn't stretched/squeezed
					texCoords.push_back( ofVec2f( t, ofMap( a, 0, 1, .5-tcOffset, 0.) ) );
					texCoords.push_back( ofVec2f( t, .5));
					texCoords.push_back( ofVec2f( t, ofMap( m, 0, 1, .5+tcOffset, 1.) ) );
					
					if(i>0){
						int prevIndex = vertices.size() - 6;
						int thisIndex = vertices.size() - 3;
						indices.push_back( prevIndex );
						indices.push_back( prevIndex+1 );
						indices.push_back( thisIndex+1 );
						indices.push_back( thisIndex );
						
						indices.push_back( prevIndex+1 );
						indices.push_back( prevIndex+2 );
						indices.push_back( thisIndex+2 );
						indices.push_back( thisIndex+1 );
					}
				}
				numIndices = indices.size();
				
				//calc the normals
				vector<ofVec3f> normals( vertices.size() );
				ofVec3f fNorm;
				int i_0, i_1, i_2, i_3;
				for(int i=0; i<numIndices; i+=4){
					i_0=indices[i], i_1=indices[i+1], i_2=indices[i+2], i_3=indices[i+3];
					
					fNorm = normalFrom4Points( vertices[i_0], vertices[i_1], vertices[i_2], vertices[i_3] );
					normals[i_0] += fNorm;
					normals[i_1] += fNorm;
					normals[i_2] += fNorm;
					normals[i_3] += fNorm;
				}
				
				for(int i=0; i<normals.size(); i++){
					normals[i].normalize();
				}
				
				vbo.setVertexData( &vertices[0], vertices.size(), GL_STATIC_DRAW );
				vbo.setNormalData( &normals[0], normals.size(), GL_STATIC_DRAW );
				vbo.setTexCoordData( &texCoords[0], texCoords.size(), GL_STATIC_DRAW );
				vbo.setIndexData( &indices[0], indices.size(), GL_STATIC_DRAW );
			}
			
		}
	}
	
	void draw( GLenum drawType = GL_QUADS, bool wireframe=false)
	{
		if(bWireframe || wireframe){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
		}
		vbo.drawElements( drawType, numIndices );
		
		if(bWireframe || wireframe){
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
		}
	}
	
	
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2)
	{
		ofVec3f norm = (p2 - p1).cross( p0 - p1);
		return norm.normalized();
	}
	
	ofVec3f normalFrom4Points(ofVec3f p0, ofVec3f p1, ofVec3f p2, ofVec3f p3)
	{
		return (normalFrom3Points(p0, p1, p2) + normalFrom3Points(p0, p2, p3)).normalized();
	}
	
	Journey* journey;
	bool needsUpdate;
	
	ofVbo vbo;
	int numIndices;
	
	ofColor color;
	
	float twistCoefficient;
	float readingScale;
	float halfMinWidth;
	
	bool bWireframe;
	
};
