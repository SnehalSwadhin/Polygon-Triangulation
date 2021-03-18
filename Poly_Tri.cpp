
/* Object Oriented Systms Design Labratory - Assignment 4
 * ------------------------------------------------------
 * Name - Snehal Swadhin
 * Roll no. - 19ME10067
 * Date - 02/11/2020
 * Topic - Polygon Triangulation O(n^2)
 */

#include <iostream>
#include <math.h>

using namespace std;

// Class defining a Point, with x and y coordinates, and index of the point in the list of vertices.
class Point
{
public:
	float x, y;
	int index;
	Point()					: x(0), y(0), index(0){} // Default constructor
	Point(float x, float y)	: x(x), y(y), index(0){} // Parameterized constructor
	Point(float x, float y, int index)	: x(x), y(y), index(index){} // Custom constructor

	// Member function to input coordinates of the point.
	get_coordinates()
	{
		cin >> x >> y;
	}

	// Member function to print coordinates of the point.
	print()
	{
		cout << '(' << x << ", " << y << ')';
	}
};

// Function to calculate distance between two point.
float diatance(Point p1, Point p2)
{
	return sqrt(pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2));
}

// Function to calculate determinant of vectors p1-p2 and p1-q.
// The sign of this determinant can be used to tell whether q lies on the left or right of vector p1-p2
float determinant(Point p1, Point p2, Point q)
{
	return (q.y-p1.y)*(p2.x-p1.x) - (p2.y-p1.y)*(q.x-p1.x);
}

// Class defining a Polygon, with a list of vertice1s (Point object array).
// It also has a list of diagonals and a boolean list of convex vertices.
class Polygon
{
private:
	int n;
	Point *vertex, *diagonal;
	bool* is_convex;
public:
	Polygon() 		: n(0){}	// Default constructor
	Polygon(int n)	: n(n)		// Parameterized constructor

	{
		vertex = new Point[n];
		is_convex = new bool[n];
		diagonal = new Point[n-3];
	}
	Polygon(int n, Point vertices[]) : n(n) // Custom constructor
	{
		vertex = vertices;
		for(int i = 0; i < n; i++)
            vertex[i].index = i;
		is_convex = new bool[n];
		diagonal = new Point[n-3];
	}

	// Function to input vertices from the user.
	void get_vertices()
	{
		cout << "Enter x and y coordinates of " << n << " vertices:" << endl;
		for(int i = 0; i < n; i++)
        {
			vertex[i].get_coordinates();
			vertex[i].index = i;
        }
	}

	// Function to delete a vertex from the list
	void delete_vertex(int index)
	{
		// Iterating over the array and shifting all the vertices after the deleted vertex.
		// This needs to be done for both the arrays.
		for(int i = index; i < n-1; i++)
		{
			vertex[i] = vertex[i+1];
			is_convex[i] = is_convex[i+1];
		}
		delete(vertex+n-1);
		delete(is_convex+n-1);
		n--; // Decreasing the number of vertices 
	}

	// Function to mark all the convex vertices
	// TIme complexity - O(n)
	void mark_convex()
	{
		float det;

		for(int i = 0; i < n-2; i++)
		{
			det = determinant(vertex[i], vertex[i+1], vertex[i+2]);
			is_convex[i+1] = det < 0;
		}

		// The last two determinants need to be calculated separately and I have not used a circular queue to store vertices 
		det = determinant(vertex[n-2], vertex[n-1], vertex[0]);
		is_convex[n-1] = det < 0;

		det = determinant(vertex[n-1], vertex[0], vertex[1]);
		is_convex[0] = det < 0;
	}

	// Function to find out and store the diagonals for triangulation of the polygon.
	void triangulize()
	{
		Polygon P = *this; // Making a copy of the polygon for calling the ear clipping function
		diagonal = P.earClipping(P.n-3);

		// Printing the diagonals found using ear clipping.
		cout << "Triangulation :" << endl;
		for(int i = 0; i < n-4; i++)
		{
			diagonal[i].print();
			cout << ", ";
		}
		diagonal[n-4].print();
		cout << "." << endl;
	}

	// Function to perform ear clipping on the  polygon and remove these "ears" in order to find diagonals of triangulation
	// Time Complexity - O(n^2) 
	Point* earClipping(int num_of_diag)
	{
		mark_convex(); // Marking the convex vertices
		int ctr = 0, i = 1;
		// ctr - Number of diagonals found till now
		// i - index of current vertex

		// Iterating over the vertices to find and remove ears from the polygon
		while(i <= n && ctr < num_of_diag)
		{
			if(is_convex[i]) // If current vertex is convex
			{
				// (i-1+n)%n gives the previous and (i+1)%n gives the next number if 0 - (n-1) are written in circular fasion
				int prev = (i-1+n)%n, next = (i+1)%n, j;

				// This loop check if the current vertex is an ear, i.e the diagonal between previous and next vertex exists
				for(j = 0; j < n; j++)
				{
					if(j != prev && j != i && j != next)
					{
						bool dir1 = determinant(vertex[prev], vertex[i], vertex[j]) < 0;
						bool dir2 = determinant(vertex[i], vertex[next], vertex[j]) < 0;
						bool dir3 = determinant(vertex[next], vertex[prev], vertex[j]) < 0;

						// If the jth point lies on the same side of each line, i.e it is inside the triangle formed by
						// prev-j-next, jth vertex is not an ear.
						if(dir1 && dir2 && dir3)
							break;
					}
				}
				if(j == n) // If the loop does not break, it means the jth vertex is an ear.
				{
					// Adding the diagonal as a pair of two indices of previous and next vertex.
					// The point object can be used here to store a pair of numbers.
					diagonal[ctr] = Point(vertex[prev].index, vertex[next].index);
					ctr++;
					delete_vertex(i); 	// Deleting the vertex. This step still does not increase the complexity of the program
										// because it is done outside the j-loop. Time comlexity is still - O(n^2)

					// Updating the is_convex array after deleting the jth vertex as it can change for the prev and next 
					// vetrtex after deletion.
					prev = (i-1+n)%n;
					next = (i+1)%n;
					is_convex[prev] = determinant(vertex[(prev-1+n)%n], vertex[prev], vertex[i]) < 0;
					is_convex[i] = determinant(vertex[prev], vertex[i], vertex[next]) < 0;
				}
				else i++; // Move to next vertex if current is not an ear
			}
			else i++; // Move to next vertex if current is not convex
		}
		return diagonal;
	}

	// Function to print the vertices of the polygon
	void print_vertices()
	{
		cout << "Vertices :" << endl;
		for(int i = 0; i < n-1; i++)
		{
			vertex[i].print();
			cout << ", ";
		}

		vertex[n-1].print();
		cout << '.' << endl;
	}
};

int main()
{
	Point vertices[] = {Point(0, 0), Point(0, 1), Point(0.5, 0.25), Point(1, 1), Point(1, 0)};
	int n = sizeof(vertices)/sizeof(Point);
	Polygon p1(n, vertices);
	p1.print_vertices();
	p1.mark_convex();
	p1.triangulize();
}

