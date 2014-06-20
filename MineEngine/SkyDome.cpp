#include "stdafx.h"
#include "SkyDome.h"
#include "Image_Utils.h"
#include <vector>

vox::SkyDome::SkyDome(void)
{
}

vox::SkyDome::~SkyDome(void)
{
}
struct PackedVertex
{
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;
        bool operator<(const PackedVertex that) const
		{
                return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
        };
};

bool getSimilarVertexIndex_fast(PackedVertex & packed, 
								std::map<PackedVertex,unsigned short> & VertexToOutIndex,
								unsigned short & result)
{
        std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
        if ( it == VertexToOutIndex.end() ){
                return false;
        }else{
                result = it->second;
                return true;
        }
}

void indexVBO(std::vector<glm::vec3> & in_vertices,
			  std::vector<glm::vec2> & in_uvs,
			  std::vector<glm::vec3> & in_normals,
			  std::vector<GLuint> & out_indices,
			  std::vector<glm::vec3> & out_vertices,
			  std::vector<glm::vec2> & out_uvs,
			  std::vector<glm::vec3> & out_normals)
{
        std::map<PackedVertex,unsigned short> VertexToOutIndex;

        // For each input vertex
        for ( unsigned int i=0; i<in_vertices.size(); i++ )
		{
				PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};
                
                // Try to find a similar vertex in out_XXXX
                unsigned short index;
                bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

                if ( found )
					out_indices.push_back( index );
				else
				{
					out_vertices.push_back( in_vertices[i]);
					out_uvs     .push_back( in_uvs[i]);
					out_normals .push_back( in_normals[i]);
					unsigned short newindex = (unsigned short)out_vertices.size() - 1;
					out_indices .push_back( newindex );
					VertexToOutIndex[ packed ] = newindex;
                }
        }
}

bool
vox::SkyDome::Init(void)
{
	InitVertex();

	texture = loadBMP_custom("skyboxCloudy.bmp");
	if (texture == 0)
		return false;
	return true;
}

bool
vox::SkyDome::InitVertex(void)
{
	float Radius = 1.0f;
	int Resolution = 100;
	std::vector<glm::vec3> v;
	std::vector<glm::vec2> t;
	std::vector<glm::vec3> n;
	float X1,Y1,X2,Y2,Z1,Z2;
	float inc1,inc2,inc3,inc4,Radius1,Radius2;
	
	for(int w = 0; w < Resolution; w++)
	{
		for(int h = (-Resolution/2); h < (Resolution/2); h++)
		{  
			inc1 = (float)(w/(float)Resolution)*2*M_PI;
			inc2 = (float)((w+1)/(float)Resolution)*2*M_PI;
			
			inc3 = (float)(h/(float)Resolution - 1)*M_PI;
			inc4 = (float)((h+1)/(float)Resolution - 1)*M_PI;
			
			X1 = sin(inc1);
			Y1 = cos(inc1);
			X2 = sin(inc2);
			Y2 = cos(inc2);
			
			// store the upper and lower radius, remember everything is going to be drawn as triangles
			Radius1 = Radius*cos(inc3);
			Radius2 = Radius*cos(inc4);
			Z1 = Radius*sin(inc3); 
			Z2 = Radius*sin(inc4);
			
			glm::vec3	v1(Radius1*X1,Z1,Radius1*Y1),
						v2(Radius1*X2,Z1,Radius1*Y2),
						v3(Radius2*X2,Z2,Radius2*Y2),
						v4(Radius1*X1,Z1,Radius1*Y1),
						v5(Radius2*X2,Z2,Radius2*Y2),
						v6(Radius2*X1,Z2,Radius2*Y1);

			v.push_back(v1);
			v.push_back(v2);
			v.push_back(v3);
			v.push_back(v4);
			v.push_back(v5);
			v.push_back(v6);
			t.push_back(glm::vec2(0.5f + glm::atan(v1.x, v1.z) / (2* M_PI), 0.5f - glm::asin(-v1.y) / M_PI));
			if (w == Resolution - 1)
			{
				t.push_back(glm::vec2(1, 0.5f - glm::asin(-v2.y) / M_PI));
				t.push_back(glm::vec2(1, 0.5f - glm::asin(-v3.y) / M_PI));
			}
			else
			{
				t.push_back(glm::vec2(0.5f + glm::atan(v2.x, v2.z) / (2* M_PI), 0.5f - glm::asin(-v2.y) / M_PI));
				t.push_back(glm::vec2(0.5f + glm::atan(v3.x, v3.z) / (2* M_PI), 0.5f - glm::asin(-v3.y) / M_PI));
			}
			t.push_back(glm::vec2(0.5f + glm::atan(v4.x, v4.z) / (2* M_PI), 0.5f - glm::asin(-v4.y) / M_PI));
			if (w == Resolution - 1)
				t.push_back(glm::vec2(1.0f, 0.5f - glm::asin(-v5.y) / M_PI));
			else
				t.push_back(glm::vec2(0.5f + glm::atan(v5.x, v5.z) / (2* M_PI), 0.5f - glm::asin(-v5.y) / M_PI));
			t.push_back(glm::vec2(0.5f + glm::atan(v6.x, v6.z) / (2* M_PI), 0.5f - glm::asin(-v6.y) / M_PI));

			// insert the normal data
			n.push_back(glm::vec3(X1,Z1,Y1) / glm::length(glm::vec3(X1,Z1,Y1)));
			n.push_back(glm::vec3(X2,Z1,Y2) / glm::length(glm::vec3(X2,Z1,Y2)));
			n.push_back(glm::vec3(X2,Z2,Y2) / glm::length(glm::vec3(X2,Z2,Y2)));

			n.push_back(glm::vec3(X1,Z1,Y1) / glm::length(glm::vec3(X1,Z1,Y1)));
			n.push_back(glm::vec3(X2,Z2,Y2) / glm::length(glm::vec3(X2,Z2,Y2)));
			n.push_back(glm::vec3(X1,Z2,Y1) / glm::length(glm::vec3(X1,Z2,Y1)));
		}
	}
	// finally, create the buffers and bind the data to them
	std::vector<GLuint> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(v, t, n, indices, indexed_vertices, indexed_uvs, indexed_normals);
	
	glGenBuffers(1, &vboVertex);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &vboUV);
	glBindBuffer(GL_ARRAY_BUFFER, vboUV);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &vboNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &vboIdx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0] , GL_STATIC_DRAW);
	
	nbTriangle = indices.size();
	
	indexed_normals.clear();
	indexed_uvs.clear();
	indexed_vertices.clear();
	indices.clear();
	n.clear();
	v.clear();
	t.clear();
	return true;
}

bool
vox::SkyDome::Render(GLint attrCoord3D, GLint attrVertexUV,
					 GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture)
{
	glDisable(GL_CULL_FACE);
	glm::mat4 Model = glm::translate(glm::mat4(), pos) * glm::scale(glm::vec3(500, 500, 500));
	glUniformMatrix4fv(attrUniformModel, 1, GL_FALSE, &Model[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(attrTexture, GL_TEXTURE0);
	
	glEnableVertexAttribArray(attrCoord3D);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
	glVertexAttribPointer(attrCoord3D, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glEnableVertexAttribArray(attrVertexNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glVertexAttribPointer(attrVertexNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glEnableVertexAttribArray(attrVertexUV);
	glBindBuffer(GL_ARRAY_BUFFER, vboUV);
	glVertexAttribPointer(attrVertexUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx);

	glDrawElements(GL_TRIANGLES, nbTriangle, GL_UNSIGNED_INT, (void*)0);
	
	glDisableVertexAttribArray(attrVertexUV);
	glDisableVertexAttribArray(attrVertexNormal);
	glDisableVertexAttribArray(attrCoord3D);
	glEnable(GL_CULL_FACE);
	return true;
}