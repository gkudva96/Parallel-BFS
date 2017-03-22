#include "mpi.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;

#define MAX_QUEUE_SIZE 5

int main(int argc, char *argv[])
{
	//Variables and Initializations
	int size, rank;
	int adjacency_matrix[100];
	int adjacency_queue[MAX_QUEUE_SIZE];
	int source_vertex;
	int no_of_vertices;
	int adjacency_row[10];
	int bfs_traversal[100];
	int visited[100];
	//MPI Code
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0)
	{
		//Entering the Number of vertices
		cout<<"Enter the number of vertices\n";
		cin>>no_of_vertices;

		//Entering the Adjacency Matrix
		cout<<"Enter the Adjacency Matrix\n";
		for(int i = 0; i < no_of_vertices * no_of_vertices; i++)
		{
			cin>>adjacency_matrix[i];
		}

		//Entering the Source Vertex
		cout<<"Enter the Source Vertex\n";
		cin>>source_vertex;
	}

	//Broadcasting the Number of vertices and the source vertex;
	MPI_Bcast(&no_of_vertices, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&source_vertex, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//Scattering each row of the adjacency matrix to each of the processes
	MPI_Scatter(adjacency_matrix, no_of_vertices, MPI_INT, adjacency_row, no_of_vertices, MPI_INT, 0, MPI_COMM_WORLD);

	//Initializing The Adjacency Queue of each process
	for(int i = 0; i < MAX_QUEUE_SIZE; i++)
	{
		adjacency_queue[i] = -1;
	}

	//BFS code
	int index = 0;
	if(rank >= source_vertex)
	{
		for(int i = 0; i < no_of_vertices; i++)
		{
			if(adjacency_row[i] == 1)
			{
				adjacency_queue[index++] = i;
			}
		}
	}

	//Gathering all the nodes in BFS found by each process
	MPI_Gather(adjacency_queue, MAX_QUEUE_SIZE, MPI_INT, bfs_traversal, MAX_QUEUE_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

	//Printing the Order of traversed nodes in root
	for(int i = 0; i < no_of_vertices; i++)
	{
		visited[i] = 0;
	}

	if(rank == 0)
	{
		cout<<source_vertex<<" ";
		for(int i = 0; i < MAX_QUEUE_SIZE * no_of_vertices; i++)
		{
			if(bfs_traversal[i] != -1)
			{
				if(visited[bfs_traversal[i]] == 0)
				{
					cout<<bfs_traversal[i]<<" ";
					visited[bfs_traversal[i]] = 1;
				}
			}
			else
			{
				continue;
			}
		}
	}

	//End of BFS code
	MPI_Finalize();
	return 0;
}