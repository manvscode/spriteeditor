#ifndef __VERTEX_H__
#define __VERTEX_H__


typedef struct tagVertex {
	union {
		struct {
			float x, y, z;
		};
		float v[3];
	};
}Vertex;

typedef Vertex VERTEX;

#endif