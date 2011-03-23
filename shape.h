// --*-c++-*--
/*

    Symmetry Groups Software
    Copyright (C) 2003  Jesse Lovelace <jllovela@ncsu.edu>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,

    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SHAPE_H
#define SHAPE_H

#include <fstream>
#include <vector>
#include <limits>
#include <exception>

#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

struct RGB
{
    int R;
    int G;
    int B;
};

struct Vertex
{
    double X;
    double Y;
    double Z;
};

class Face
{
public:
    Face() {};

    vector<int> verts;
};

class Shape
{
    ofstream out;
public:

    Shape(const string& name): m_selectedFace(-1) 
    { 
        out.open("shapelog.txt");
        
        bool loadOK = false;
        try
        {
            loadOK = loadOff(name, m_verts, m_faces);
        }
        catch( exception &e)
        {
            throw e;
        }

        if (!loadOK)
            throw exception("WTF");

        makeGenLists();
    }

    virtual ~Shape()
    {
        glDeleteLists(m_list, (GLsizei)m_faces.size()*2); 

    }

    enum Axis { X, Y, Z };

    /// Each shape returns its string name
    virtual string name() = 0;

    // Draws the shape to the current viewport
        // Draws the shape to the current viewport
    virtual void render();

    /// Set the rotation of the solid
    void setViewRotation(int x, int y);

    // Rotates a certain face about a certain axis
    // a certain number of degrees
    void rotate(int face, Axis axis, int degrees);

    // selects the base face for calculating orb and stab
    // only one face is selected at a time.
    virtual void selectFace(unsigned int face, const RGB& color);

    /// Creates the OpenGL lists for this solid
    ///  (call only once)
    void makeGenLists();
    
    // highlights a face for use in showing stab and orb
    // only one face is highlighted at a time.
    virtual void highlightFace(vector<int> faces, RGB color);

    int getSelectedFace() { return m_selectedFace; }
    /// find the orbit of the currently selected face
    /// and store it
    virtual void findOrbit();

    /// clear the calculated orbit, used when changing faces
    void clearOrbit() { m_orbit.clear(); }

    /// Returns a list of indises of the orbit for the 
    ///  currently selected face.
    vector<int>& getOrbit() { return m_orbit; }

    /// The length from point a to point b
    double length(const Vertex& a, const Vertex& b);

    /// Caclulates if a and b are equal to a certain precision
    bool equal(const double& a, const double& b);

    /// Calculates the cicumference of a face
    double circumference(const Face& face);

    /// Are all sides of equal length
    bool isRegular(const Face& face);

    /// Tries :> to see if two faces are the same
    bool polySame(const Face& face1, const Face& face2);

    /// converts a Face to a vertex array
    vector<Vertex> face2Vertex(const Face& face);

    /// Draws the axises in OpenGL
    void drawAxes();

    /// Returns a normal vector of a face 
    ///  (always pointing away from origin!)
    Vertex getNormal(const Face& face);

    /// Makes the opengl call setting a normal for a face
    void setNormal(const Face& face); 

    /// Returns the face array
    vector<Face>& getFaces() { return m_faces; }

    /// Draws a line begining at the origin passing through
    ///  the center of the selected face
    void drawStabLine();

    /// Calculate stabilizer degrees for current face
    void calcStab();

    /// Determin if this rotation stabilizes the face
    bool isStab(const double& rot);

    void Verts2Vector(const vector<Vertex>& verts, vector<float>& vec);

    void Vector2Verts(const vector<float>& vec, vector<Vertex>& verts);

    void calcStabAxis();

    vector<double>& getStabs() { return m_stabs; }

private:

    /// Parses a .OFF file into face and vertex data arrays
    static bool loadOff(const string& filename, vector<Vertex>& verts, vector<Face>& vFace);
  
    /// Checks to see if a face (by its index) is highlighted
    bool isHighlighted(const int face) const;

    GLuint	m_list;
    GLfloat m_rot;

    vector<Vertex> m_verts;
    vector<Face> m_faces;

    vector<int> m_orbit;

    Vertex m_stabLine;
    
    /// Stabelizer degrees for selected face
    vector<double> m_stabs;

    /// relative mouse-base rotations
    int m_xrot;
    int m_yrot;
    
    int m_selectedFace;
    RGB m_selectedColor;

    /// rotational vector of currently selected face
    ///  for stabilizer calculations
    Vertex m_rotationalVector;

    vector<int> m_highlightFace;
    RGB m_highlightColor;

    float area3D_Polygon(const vector<Vertex>& V, const Vertex& N );

};

class Cube: public Shape
{
public:
    virtual string name() { return "Cube"; }
    Cube(): Shape("off/cube.off")
    {}
};

class Cuboctahedron: public Shape
{
public:
    virtual string name() { return "Cuboctahedron"; }
    Cuboctahedron(): Shape("off/cuboctahedron.off")
    {}
};


class Icosidodecahedron: public Shape
{
public:
    virtual string name() { return "Icosidodecahedron"; }
    Icosidodecahedron(): Shape("off/icosidodecahedron.off") 
    {}

};

class GRhombicuboctahedron: public Shape
{
public:
    virtual string name() { return "Great Rhombicuboctahedron"; }
    GRhombicuboctahedron(): Shape("off/great_rhombicuboctahedron.off")
    {}

};

class Pentakisdodecahedron: public Shape
{
public:
    virtual string name() { return "Penta Kisdodecahedron"; }
    Pentakisdodecahedron(): Shape("off/pentakisdodecahedron.off")
    {}

};

class RhombicDodecahedron: public Shape
{
public:
    virtual string name() { return "Rhombic Dodecahedron"; }
    RhombicDodecahedron(): Shape("off/rhombic_dodecahedron.off") 
    {}
};

class TruncatedDodecahedron: public Shape
{
public:
    virtual string name() { return "Truncated Dodecahedron"; }
    TruncatedDodecahedron(): Shape("off/truncated_dodecahedron.off")
    {}
};

class SnubCuboctahedron: public Shape
{
public:
    virtual string name() { return "Snub Cuboctahedron"; }
    SnubCuboctahedron(): Shape("off/snub_cuboctahedron.off")
    {}
};

class SnubIcosidodecahedron: public Shape
{
public:
    virtual string name() { return "Snub Icosidodecahedron"; }
    SnubIcosidodecahedron(): Shape("off/snub_icosidodecahedron.off")
    {}
};

class TruncatedIcosahedron: public Shape
{
public:
    virtual string name() { return "Truncated Icosahedron"; }
    TruncatedIcosahedron(): Shape("off/truncated_icosahedron.off")
    {}
};




class Octahedron: public Shape
{
public:
    virtual string name() { return "TOctahedron"; }
    Octahedron(): Shape("off/octahedron.off")
    {}
};
/*
disdyakis_dodecahedron.off
disdyakis_triacontahedron.off
great_rhombicosidodecahedron.off
pentagonal_hexecontahedron.off
pentagonal_icositetrahedron.off
rhombic_dodecahedron.off
rhombic_triacontahedron.off
rhombicosidodecahedron.off
rhombicuboctahedron.off
tetrakishexahedron.off
trapezoidal_hexecontahedron.off
trapezoidal_icositetrahedron.off
triakisicosahedron.off
triakisoctahedron.off
truncated_cube.off
truncated_octahedron.off*/

#endif

