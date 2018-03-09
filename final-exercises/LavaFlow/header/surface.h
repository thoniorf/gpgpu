#ifndef SURFACE_H
#define SURFACE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>



struct flow_data {
    float x;
    float altitude;
    float z;
    float temperature;

};

struct normal_data {
    float x;
    float y;
    float z;
    int occurences;

};

class Surface {
public:
    Surface() {}

    void setCellSize(int cellSize)
    {
        cell_size = cellSize;
    }


    void DeleteGridsAndDirtyNormals()
    {
        for(int i = 0; i < data_rows; ++i)
        {
            delete [] data_grid[i];
        }
        delete data_grid;

        for(int i = 0; i < green_rows; ++i)
        {
            delete [] green_grid[i];
        }
        delete green_grid;

        for(int i = 0; i < green_rows; ++i)
        {
            delete [] normals_dirty[i];
        }
        delete normals_dirty;
    }

//==== DATA GRID METHODS
    void createDataGrid(int row, int cols)
    {
        this->data_rows = row;
        this->data_cols = cols;

        data_grid = new flow_data*[data_rows];
        for(int i = 0; i<data_rows; i++)
        {
            data_grid[i] = new flow_data[data_cols];
        }

    }

    void setDataCoordAltitiudeAtPosition(float x, float z, float altitude, int i,int j) {
        data_grid[i][j].x = x;
        data_grid[i][j].z = z;
        data_grid[i][j].altitude = altitude;
    }

    void addDataThicknessAtIndex(float thickness,int i , int j){
        data_grid[i][j].altitude += thickness;
    }
    void setDataTemperatureAtIndex(float temperature, int i, int j) {
        data_grid[i][j].temperature = temperature;
    }

    float getDataTemperatureAtIndex(int i, int j)
    {
        return data_grid[i][j].temperature;
    }


//==== GREEN GRID METHODS
    void createGreenGrid() {
        this->green_rows = data_rows + 1;
        this->green_cols = data_cols + 1;

        green_grid = new flow_data* [green_rows];
        for(int i = 0;i< green_rows; i++)
        {
            green_grid[i] = new flow_data[green_cols];
        }
    }

    void generateGreenCoord() {
        for (unsigned int i = 0; i < green_rows; i++)
        {
            for (unsigned int j = 0; j < green_cols; j++)
            {
                green_grid[i][j].x = (float) (i*cell_size-(cell_size/2));
                green_grid[i][j].z = (float) (j*cell_size-(cell_size/2));

            }
        }
    }

    void setAltitudeFromDataToGreen() {
        // ASSIGN FOUR CORNER
        green_grid[0][0].altitude = data_grid[0][0].altitude;
        green_grid[data_rows][0].altitude = data_grid[data_rows-1][0].altitude;
        green_grid[0][data_cols].altitude = data_grid[0][data_cols-1].altitude;
        green_grid[data_rows][data_cols].altitude = data_grid[data_rows-1][data_cols-1].altitude;
        // ASSIGN  ROW BORDER
        for(int i = 1; i < data_rows; i++){
            green_grid[i][0].altitude = (data_grid[i-1][0].altitude+data_grid[i][0].altitude)/2;
            green_grid[i][data_cols].altitude = (data_grid[i-1][data_cols-1].altitude+data_grid[i][data_cols-1].altitude)/2;
        }
        // ASSIGN COL BORDER
        for(int j = 1; j < data_cols; j++){
            green_grid[0][j].altitude = (data_grid[0][j-1].altitude+data_grid[0][j].altitude)/2;
            green_grid[data_rows][j].altitude = (data_grid[data_rows-1][j-1].altitude+data_grid[data_rows-1][j].altitude)/2;
        }
        // ASSIGN INNER BORDER
        for(int i = 1; i < data_rows; i++) {
            for(int j = 1; j < data_cols; j++) {
                green_grid[i][j].altitude = (data_grid[i-1][j-1].altitude+data_grid[i][j-1].altitude+data_grid[i-1][j].altitude+data_grid[i][j].altitude)/4;
            }
        }
    }
    void setTemperatureFromDataToGreen() {
        // ASSIGN FOUR CORNER
        green_grid[0][0].temperature = data_grid[0][0].temperature;
        green_grid[data_rows][0].temperature = data_grid[data_rows-1][0].temperature;
        green_grid[0][data_cols].temperature = data_grid[0][data_cols-1].temperature;
        green_grid[data_rows][data_cols].temperature = data_grid[data_rows-1][data_cols-1].temperature;
        // ASSIGN  ROW BORDER
        for(int i = 1; i < data_rows; i++){
            green_grid[i][0].temperature = (data_grid[i-1][0].temperature+data_grid[i][0].temperature)/2;
            green_grid[i][data_cols].temperature = (data_grid[i-1][data_cols-1].temperature+data_grid[i][data_cols-1].temperature)/2;
        }
        // ASSIGN COL BORDER
        for(int j = 1; j < data_cols; j++){
            green_grid[0][j].temperature = (data_grid[0][j-1].temperature+data_grid[0][j].temperature)/2;
            green_grid[data_rows][j].temperature = (data_grid[data_rows-1][j-1].temperature+data_grid[data_rows-1][j].temperature)/2;
        }
        // ASSIGN INNER BORDER
        for(int i = 1; i < data_rows; i++) {
            for(int j = 1; j < data_cols; j++) {
                green_grid[i][j].temperature = (data_grid[i-1][j-1].temperature+data_grid[i][j-1].temperature+data_grid[i-1][j].temperature+data_grid[i][j].temperature)/4;
            }
        }
    }

//==== VERTICES
    void setVertexFromGreen(){
        vertices = new float[green_rows*green_cols*3]; // three are the vertex components
        int index = 0;
        for(int i = 0; i < green_rows; i++)
        {
            for(int j = 0; j < green_cols; j++)
            {
                vertices[index++] = green_grid[i][j].x;
                vertices[index++] = green_grid[i][j].altitude;
                vertices[index++] = green_grid[i][j].z;
            }
        }
    }

//==== INDICES
    void setIndicesFromGreen(){
        indices = new unsigned int[green_rows*green_cols*6];
        int index = 0;
        /*
         * 1 2
         * 3
         */
        for(int i = 0; i < green_rows - 1; i++)
        {
            for(int j = 0; j < green_cols - 1; j++)
            {
                indices[index++] = (i*green_cols)+j;
                indices[index++] = (i*green_cols)+(j+1);
                indices[index++] = ((i+1)*green_cols)+j;
            }
        }
        /*
         * 1
         * 2 3
         */
        for(int i = 1; i < green_rows; i++)
        {
            for(int j = 0; j < green_cols-1; j++)
            {
                indices[index++] = (i*green_cols) + j;
                indices[index++] = ((i-1)* green_cols) + (j+1);
                indices[index++] = (i* green_cols) + (j+1);
            }
        }
    }

//==== TEXUTURES
    void setTextureCoordsFromGreen(){
        textCoords = new float[green_rows*green_cols*2]; // two are texture components
        int index = 0;
        for(int i = green_rows - 1; i >=0 ; i--)
        {
            for(int j = 0; j < green_cols; j++)
            {
                textCoords[index++] = (cell_size*j)/(cell_size*(green_cols-1));
                textCoords[index++] = ((cell_size*(green_rows-1))-(cell_size*i))/(cell_size*(green_rows-1));
            }
        }
    }

//==== COLORS
    void setColorsFromGreen(){
        colors = new float[green_cols*green_rows*3]; // three are colors components
        int index = 0;
        for(int i = 0; i < green_rows; i++)
        {
            for(int j = 0; j < green_cols; j++)
            {
                colors[index++] = 1.0f;
                colors[index++] = green_grid[i][j].temperature;
                colors[index++] = 0.0f;
            }
        }
    }

//===== NORMALS
    void calculateDirtyNormals(){
        normals_dirty = new normal_data* [green_rows];
        for(int i = 0; i < green_rows; i++)
        {
            normals_dirty[i] = new normal_data[green_cols];
        }
        int next_j= 0, next_i = 0, previous_i = 0;
        /*
         * 1 2
         * 3
         */
        for(int i = 0; i < green_rows-1; i++)
        {

            for(int j = 0; j < green_cols-1; j++)
            {
                next_i = i+1;
                next_j = j+1;
                glm::vec3 first = glm::vec3(
                            (green_grid[i][j]).x,
                            (green_grid[i][j]).altitude,
                            (green_grid[i][j]).z
                            );
                glm::vec3 second = glm::vec3(
                            (green_grid[i][next_j]).x,
                            (green_grid[i][next_j]).altitude,
                            (green_grid[i][next_j]).z
                            );
                glm::vec3 third = glm::vec3(
                            (green_grid[next_i][j]).x,
                            (green_grid[next_i][j]).altitude,
                            (green_grid[next_i][j]).z
                            );

                glm::vec3 V = third - first;
                glm::vec3 U = second - first;

                glm::vec3 normal = glm::cross(U, V);

                normals_dirty[i][j].x += normal.x;
                normals_dirty[i][j].y += normal.y;
                normals_dirty[i][j].z += normal.z;
                normals_dirty[i][j].occurences += 1;

                normals_dirty[i][next_j].x += normal.x;
                normals_dirty[i][next_j].y += normal.y;
                normals_dirty[i][next_j].z += normal.z;
                normals_dirty[i][next_j].occurences += 1;

                normals_dirty[next_i][j].x += normal.x;
                normals_dirty[next_i][j].y += normal.y;
                normals_dirty[next_i][j].z += normal.z;
                normals_dirty[next_i][j].occurences += 1;

            }
        }
        /*
         * 1
         * 2 3
         */
        for(int i = 1; i < green_rows; i++)
        {

            for(int j = 0; j < green_cols-1; j++)
            {
                next_j = j+1;
                previous_i = i-1;
                glm::vec3 first = glm::vec3(
                            (green_grid[i][j]).x,
                            (green_grid[i][j]).altitude,
                            (green_grid[i][j]).z
                            );
                glm::vec3 second = glm::vec3(
                            (green_grid[previous_i][next_j]).x,
                            (green_grid[previous_i][next_j]).altitude,
                            (green_grid[previous_i][next_j]).z
                            );
                glm::vec3 third = glm::vec3(
                            (green_grid[i][next_j]).x,
                            (green_grid[i][next_j]).altitude,
                            (green_grid[i][next_j]).z
                            );

                glm::vec3 V = third - first;
                glm::vec3 U = second - first;

                glm::vec3 normal = glm::cross(U, V);

                normals_dirty[i][j].x += normal.x;
                normals_dirty[i][j].y += normal.y;
                normals_dirty[i][j].z += normal.z;
                normals_dirty[i][j].occurences += 1;

                normals_dirty[previous_i][next_j].x += normal.x;
                normals_dirty[previous_i][next_j].y += normal.y;
                normals_dirty[previous_i][next_j].z += normal.z;
                normals_dirty[previous_i][next_j].occurences += 1;

                normals_dirty[i][next_j].x += normal.x;
                normals_dirty[i][next_j].y += normal.y;
                normals_dirty[i][next_j].z += normal.z;
                normals_dirty[i][next_j].occurences += 1;

            }
        }
    }

    void calculateNormals()
    {
        normals = new float[green_rows*green_cols*3]; // three are normals components
        int index = 0;
        for(int i = 0; i < green_rows; i++)
        {
            for(int j= 0; j < green_cols; j++)
            {
                // normalize by occurrences, because of duplicated dirty normals
                normals[index++] = normals_dirty[i][j].x / normals_dirty[i][j].occurences;
                normals[index++] = normals_dirty[i][j].y / normals_dirty[i][j].occurences;
                normals[index++] = normals_dirty[i][j].z / normals_dirty[i][j].occurences;
            }
        }
    }

    unsigned int getGreenGridElementsNumber()
    {
        return green_rows*green_cols;
    }

    float* vertices;
    unsigned int* indices;
    float* textCoords;
    float* colors;
    float* normals;

private:
    int data_rows,data_cols;
    int green_rows, green_cols;
    float cell_size;
    flow_data** data_grid;
    flow_data** green_grid;
    normal_data** normals_dirty; // for every triangle , even duplicated
};
#endif // SURFACE_H
