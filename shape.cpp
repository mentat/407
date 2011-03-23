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
#include <fstream>
#include <vector>
#include <limits>
#include <exception>

#include "windows.h"
#include "wx/timer.h"
#include "GL/gl.h"
#include "GL/glu.h"

#include "shape.h"

using namespace std;

// area3D_Polygon(): computes the area of a 3D planar polygon
//    Input:  int n = the number of vertices in the polygon
//            Point* V = an array of n+2 vertices in a plane
//                       with V[n]=V[0] and V[n+1]=V[1]
//            Point N = unit normal vector of the polygon's plane
//    Return: the (float) area of the polygon
// From: http://geometryalgorithms.com/Archive/algorithm_0101/

float
Shape::area3D_Polygon(const vector<Vertex>& V, const Vertex& N )
{
    float area = 0;
    float an, ax, ay, az;  // abs value of normal and its coords
    int   coord;           // coord to ignore: 1=x, 2=y, 3=z
    unsigned int   i, j, k;         // loop indices

    // select largest abs coordinate to ignore for projection
    ax = (N.X>0 ? N.X : -N.X);     // abs x-coord
    ay = (N.Y>0 ? N.Y : -N.Y);     // abs y-coord
    az = (N.Z>0 ? N.Z : -N.Z);     // abs z-coord

    coord = 3;                     // ignore z-coord
    if (ax > ay) {
        if (ax > az) coord = 1;    // ignore x-coord
    }
    else if (ay > az) coord = 2;   // ignore y-coord

    // compute area of the 2D projection
    for (i=1, j=2, k=0; i<= V.size(); i++, j++, k++)
        switch (coord) {
        case 1:
            area += (V[i].Y * (V[j].Z - V[k].Z));
            continue;
        case 2:
            area += (V[i].X * (V[j].Z - V[k].Z));
            continue;
        case 3:
            area += (V[i].X * (V[j].Y - V[k].Y));
            continue;
        }

    // scale to get area before projection
    an = sqrt( ax*ax + ay*ay + az*az);  // length of normal vector
    switch (coord) {
    case 1:
        area *= (an / (2*ax));
        break;
    case 2:
        area *= (an / (2*ay));
        break;
    case 3:
        area *= (an / (2*az));
    }
    return area;
}

bool inline Shape::isHighlighted(const int face) const
{
    for (unsigned int i = 0; i < m_highlightFace.size(); i++)
    {
        if (m_highlightFace[i] == face)
            return true;
    }
    return false;
}

void Shape::render()
{
    drawAxes();
    drawStabLine();
    
    for (unsigned int i = 0; i < m_faces.size(); i++)
    {
        if (i == m_selectedFace)
            glColor4f(0.7f,0.0f,0.0f,0.7f);
        else
            if (isHighlighted(i))
                glColor4f(0.0f, 0.8f, 0.0f, 1.0f);
            else
                glColor4f(0.0f,0.0f,0.5f,0.5f);

        glCallList(m_list + i);

        /// Draw skeleton
        glColor4f(0.0f,0.5f,0.0f,1.0f);
        glCallList(m_list + i + m_faces.size());

    }
}

void Shape::calcStabAxis()
{
    if (m_selectedFace == -1)
    return;

//if (isRegular(m_faces[m_selectedFace]))
//{
    out << "Drawing stab line, regular" << endl;
    double x = 0.0, y = 0.0, z = 0.0;

    for (unsigned int i = 0; i < m_faces[m_selectedFace].verts.size(); i++)
    {

        x += m_verts[m_faces[m_selectedFace].verts[i]].X;
        y += m_verts[m_faces[m_selectedFace].verts[i]].Y;
        z += m_verts[m_faces[m_selectedFace].verts[i]].Z;
    }

    /// Average these
    x /= float(m_faces[m_selectedFace].verts.size());
    y /= float(m_faces[m_selectedFace].verts.size());
    z /= float(m_faces[m_selectedFace].verts.size());


    m_stabLine.X = x;
    m_stabLine.Y = y;
    m_stabLine.Z = z;

     //}
    //else
   // {
        /// ahh shit, find the centroid

    //}


}

void Shape::drawStabLine()
{
    if (m_selectedFace == -1)
        return;

    calcStabAxis();

    glColor3f(0.2f,1.0f,1.0f); 
    //glColor4i(246,255,8,128);
    glBegin(GL_LINE_STRIP);
	glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(float(m_stabLine.X) * 1.5, float(m_stabLine.Y)* 1.5, float(m_stabLine.Z)* 1.5);
    glEnd();


}

bool Shape::loadOff(const string& filename, vector<Vertex>& verts, vector<Face>& vFace)
{
    ofstream out;
    out.open("log.txt");

    verts.clear();
    vFace.clear();

    ifstream in;
    in.open(filename.c_str());

    if (in.fail() || in.bad())
    {
        throw exception("Cannot open OFF file.");
        return false;
    }
    

    /// move past comments
    while (in && (in.peek() == '#'))
    {
        in.ignore(500, '\n');
    }

    int vertices, faces, edges;
    int bad_input;

    // get vertices, faces, and edges from spec
    do {
        bad_input=0;
        in >> vertices;
        in >> faces;
        in >> edges;

        if (!in)
        {
            bad_input=1;
            in.clear();
            in.ignore(500,'\n');
        }
    }
    while (bad_input && in);

    out << "Vertices: " << vertices << endl;
    out << "Faces: " << faces << endl;
    out << "Edges: " << edges << endl;

    // get the coordinates for all vetices
    double x, y, z;

    out << "Coords are: " << endl;

    for (int i = 0; i < vertices; i++)
    {
        if (!in)
        {
            throw exception("Cannot get vertices coords.");
            return false;
        }

        in >> x;
        in >> y;
        in >> z;

        if (!in)
        {          
            throw exception("Cannot read vertices!");
        }
        else
        {
            Vertex temp;
            temp.X =x;
            temp.Y =y;
            temp.Z =z;

            out <<  "Float " << x << " " << y << " " << z << endl;

            verts.push_back(temp);
        }

        /// ignor other stuff on line
        //in.ignore(20, '\n');

    }

    /// now get descriptions for all faces
    for (int i = 0; i < faces; i++)
    {
        //if (!in)
        // {
            //   throw exception("Cannot get face discriptions!");
        //     return false;
        //  }

        int Nv; // number of vertices on this face

        in >> Nv;

        out << "Number of verts on face: " << i << " is: " << Nv << endl;

        if (!in)
        {
            throw exception("Cannot get number of verts on face.");
            return false;
        }

        int vIndex;
        Face tempFace;

        /// gather each vert index for the given face
        for (int j = 0; j < Nv; j++)
        {
            in >> vIndex;
            out << vIndex << " ";

            if (!in)
            {
                throw exception("Cannot read in face vertext index loop.");
                return false;
            }

            // push the vert index into the face
            tempFace.verts.push_back(vIndex);
        }
        out << endl;

        // push this face onto face vector
        vFace.push_back(tempFace);


        // skip colorspec and go to next line
        in.ignore(20, '\n');
    }

    return true;

}

void Shape::setNormal(const Face& face)
{
    if (face.verts.size() < 3)
        return; // this would be not good
    
    Vertex r = getNormal(face);

    glNormal3f(r.X, r.Y, r.Z);
    
}

Vertex Shape::getNormal(const Face& face)
{
    Vertex ret;

    if (face.verts.size() < 3)
        return ret; // this would be not good

    double normalX = 
        ( m_verts[face.verts[0]].Y - m_verts[face.verts[1]].Y )
        * ( m_verts[face.verts[1]].Z - m_verts[face.verts[2]].Z )
        - ( m_verts[face.verts[0]].Z - m_verts[face.verts[1]].Z )
        * ( m_verts[face.verts[1]].Y - m_verts[face.verts[2]].Y );

    double normalY = 
        ( m_verts[face.verts[0]].Z - m_verts[face.verts[1]].Z )
        * ( m_verts[face.verts[1]].X - m_verts[face.verts[2]].X )
        - ( m_verts[face.verts[0]].X - m_verts[face.verts[1]].X )
        * ( m_verts[face.verts[1]].Z - m_verts[face.verts[2]].Z );

    double normalZ = 
        ( m_verts[face.verts[0]].X - m_verts[face.verts[1]].X )
        * ( m_verts[face.verts[1]].Y - m_verts[face.verts[2]].Y )
        - ( m_verts[face.verts[0]].Y - m_verts[face.verts[1]].Y )
        * ( m_verts[face.verts[1]].X - m_verts[face.verts[2]].X );

    double unit = sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);

    ret.X = normalX/unit;
    ret.Y = normalY/unit;
    ret.Z = normalZ/unit;

    return ret;

}

void Shape::drawAxes()
{

    glColor3f(1.0f,1.0f,1.0f);
    glBegin(GL_LINE_STRIP);
	glVertex3f(0.0f, 1.5f, 0.0f);
    glVertex3f(0.0f, -1.5f, 0.0f);
    glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(1.5f, 0.0f, 0.0f);
    glVertex3f(-1.5f, 0.0f, 0.0f);
    glEnd();

    glBegin(GL_LINE_STRIP);
	glVertex3f(0.0f, 0.0f, 1.5f);
    glVertex3f(0.0f, 0.0f, -1.5f);
    glEnd();
}

void Shape::makeGenLists()
{

    /// create a GL list for EACH FACE * 2 for shell
    m_list = glGenLists((GLsizei)m_faces.size()*2);

    for (unsigned int i = 0; i < m_faces.size(); i++)
    {
        
        /// compile solid
        glNewList(m_list + i, GL_COMPILE);

        glBegin(GL_POLYGON);
        
        /// inner vertex draw loop
        for (unsigned int j = 0; j < m_faces[i].verts.size(); j++)
        {
            glVertex3f( 
                m_verts[(m_faces[i].verts[j])].X, 
                m_verts[(m_faces[i].verts[j])].Y, 
                m_verts[(m_faces[i].verts[j])].Z);
        }

        glEnd();
        glEndList();

        /// Compile skeleton
        glNewList(m_list + i + m_faces.size(), GL_COMPILE);

        glBegin(GL_LINE_STRIP);
        
        /// inner vertex draw loop
        for (unsigned int j = 0; j < m_faces[i].verts.size(); j++)
        {
            glVertex3f( 
                m_verts[(m_faces[i].verts[j])].X, 
                m_verts[(m_faces[i].verts[j])].Y, 
                m_verts[(m_faces[i].verts[j])].Z);
        }

        glEnd();
        glEndList();
    }

}

bool Shape::polySame(const Face& face1, const Face& face2)
{

    if (face1.verts.size() == face2.verts.size())
    {
        out << "Same number of verts for faces " << (unsigned int)face2.verts.size() << endl;
        if (isRegular(face1))
        {
            out << "First face regular.\n";
            if (isRegular(face2))
            {  
                out << "Second face regular.\n";
                // if both are regular just compair circumference
                if (equal(circumference(face1), circumference(face2)))
                {
                    out << "Circ same\n";
                    return true;
                }
                else 
                {
                    out << "Circ not same.\n";
                    return false;
                }
            }
            else // one is regular and one not, cannot be equal
            {
                out << "One regular one not.\n";
                return false;
            }
        }
        else
        {
            out << "Falling back to area.\n";
            if (equal(area3D_Polygon(face2Vertex(face1), getNormal(face1)),
                area3D_Polygon(face2Vertex(face2), getNormal(face2))) )
                return true;
            else 
                return false;
        }
    }
    else
        return false;

}

bool Shape::isRegular(const Face& face)
{
    if (face.verts.size() < 3)
    {
        out << "Not enough faces.\n";
        return false;
    }

    /// try to get rid of round-off error by truncation
    double tmp = length(m_verts[face.verts[0]], 
        m_verts[face.verts[1]]);

    out << "Ref length is: " << tmp << endl;

    for (unsigned int i = 2; i < face.verts.size(); i++)
    {
        if ( !equal( length(m_verts[face.verts[i-1]],
            m_verts[face.verts[i]]) , tmp) )
        {
            out << "Length " << i << " " << i-1 << " is: " << tmp << endl;
            return false;
        }
    }
    /// now do from size-1 to 0

    if (!equal(length(m_verts[face.verts[face.verts.size() - 1]],
        m_verts[face.verts[0]]),tmp))
        return false;

    /// that should be all of them

    return true;
}


double Shape::circumference(const Face& face)
{

    /// can't do circ of 2 points
    if (face.verts.size() < 3)
        return 0;

    double tmp = 0.0;
    for (unsigned int i = 1; i < face.verts.size(); i++)
    {
        tmp += length(m_verts[face.verts[i-1]],
            m_verts[face.verts[i]]);
    }

    tmp += length(m_verts[face.verts[face.verts.size() - 1]],
        m_verts[face.verts[0]]);

    out << "Circ is: " << tmp << endl;
    return tmp;

}

void Shape::findOrbit()
{
    if (m_selectedFace == -1)
        return;

    m_orbit.clear();
    m_highlightFace.clear();

    for (unsigned int i = 0; i < m_faces.size(); i++)
    {
        if (polySame(m_faces[m_selectedFace], m_faces[i]))
            m_orbit.push_back(i);
    }
}

void Shape::highlightFace(vector<int> faces, RGB color)
{
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        if ((faces[i] < 0) || (faces[i] >= m_faces.size()))
            return;
    }
    m_highlightFace.clear();

    m_highlightFace = faces;
    m_highlightColor = color;

}

vector<Vertex> Shape::face2Vertex(const Face& face)
{
    vector<Vertex> v;

    for (unsigned int i = 0 ; i < face.verts.size(); i++)
    {
        Vertex tmp;
        tmp.X = m_verts[face.verts[i]].X;
        tmp.Y = m_verts[face.verts[i]].Y;
        tmp.Z = m_verts[face.verts[i]].Z;
        v.push_back(tmp);
    }
    return v;

}

double Shape::length(const Vertex& a, const Vertex& b)
{
    // slow, square roots are the devil
    double x = a.X - b.X;
    double y = a.Y - b.Y;
    double z = a.Z - b.Z;

    return sqrt(x*x + y*y + z*z);

}


void Shape::selectFace(unsigned int face, const RGB& color)
{
    if (face >= m_faces.size())
        return;

    m_selectedFace = face;
    m_selectedColor = color;
    m_orbit.clear();
    m_highlightFace.clear();

}

bool Shape::equal(const double& a, const double& b)
{
    const int tolerance = 10000;

    if (int(tolerance * a) == int(tolerance * b))
        return true;
    return false;
}

void Shape::calcStab()
{
    if (m_selectedFace == -1)
        return;

    m_stabs.clear();

    if (isRegular(m_faces[m_selectedFace]))
    {
        /// thank the lord!
        /// Tje identity is always in the stab
        m_stabs.push_back(0);

        for (unsigned int i = 1; i < m_faces[m_selectedFace].verts.size(); i++)
        {
            double degrees = 360.0 / double(m_faces[m_selectedFace].verts.size());
            degrees *= i;
            
            if (isStab(degrees))
                m_stabs.push_back(degrees);


        }
    }
    else
        m_stabs.push_back(0);

}

bool Shape::isStab(const double& rot)
{
    if (m_selectedFace == -1)
        return false;

    calcStabAxis();

    /// making a copy of the entire set of vertexes
    vector<Vertex> rotatedVerts = m_verts;

    /// a Copy of the current face.  the coordinates
    /// of this must match after rotation
    Face rotatedFace = m_faces[m_selectedFace];

    /// an array to pass to the OGL vertext processor
    GLdouble crap[16];

    /// get a big long list of numbers
    vector<float> allverts;
    Verts2Vector(rotatedVerts, allverts);

    out << "Total vertices: " << (unsigned int)rotatedVerts.size() << endl;
    out << "Total allverts size: " << (unsigned int)allverts.size() << endl;

    /// a big long list of new numbers
    vector<float> newverts;

    /// The correct loop size...
    unsigned int loopsize = (int)allverts.size() / 16;

    if ((allverts.size() % 16) > 0)
        loopsize++;

    out << "Loop size is: " << loopsize << endl;


    /// the real number of floats countdown
    unsigned int validsize = 0;

    glEnable(GL_NORMALIZE);

    //// 
    for (unsigned int i = 0; i < loopsize; i++)
    {
        /// fill the first matrix
        for (unsigned int k = 0; k < 16; k++)
        {
            if (validsize < allverts.size())
            {
                out << allverts[validsize] << endl;
                crap[k] = allverts[validsize++];
            }
            else
                crap[k] = 0;
            
        }
        out << "------" << endl;

        /// Let OGL do the processing
        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();
        glRotatef(rot, m_stabLine.X, m_stabLine.Y, m_stabLine.Z);
        glMultMatrixd(crap);
        glGetDoublev(GL_MODELVIEW, crap);
        glPopMatrix();

        /// Fill in the new matrix
        for (unsigned int k = 0; k < 16; k++)
        {
            if (newverts.size() < allverts.size())
            {
                out << crap[k] << endl;
                newverts.push_back(crap[k]);
            }
            else
                break;
        }
        out << endl;

    }

    for (unsigned int outer = 0; outer < newverts.size(); outer++)
    {
        bool found = false;

        for (unsigned int inner = 0; inner < allverts.size(); inner++)
        {

            if (equal(newverts[outer],allverts[inner]))
            {
                /// effectively remove this vert from array
                /// so mapping is 1-1
                allverts[inner] = -1000.0;
                found = true;
                break;
            }
            
  
        }

        if (!found)
        {
            out << "Not found" << endl;
            return false;
        }

    }

    out << "Ended up with: " << (unsigned int)newverts.size() << " doubles." << endl;
    return true;

    

}

void Shape::Verts2Vector(const vector<Vertex>& verts, vector<float>& vec)
{
    vec.clear();

    for (unsigned int i = 0; i < verts.size(); i++)
    {
        vec.push_back(verts[i].X);
        vec.push_back(verts[i].Y);
        vec.push_back(verts[i].Z);
    }

}

void Shape::Vector2Verts(const vector<float>& vec, vector<Vertex>& verts)
{
    verts.clear();

    for (unsigned int i = 0; i < vec.size(); i += 3)
    {
        Vertex tmp;
        tmp.X = vec[i];
        tmp.Y = vec[i + 1];
        tmp.Z = vec[i + 2];

        verts.push_back(tmp);
    }

}
