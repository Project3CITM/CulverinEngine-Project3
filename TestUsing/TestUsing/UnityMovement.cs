using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovementTest : MonoBehaviour {

    private float movSpeed = 5.0f;
    public  Vector3 endPosition;
    int distanceToMove = 10;
    public int[,] array2Da;
    public int curr_x = 0;
    public int curr_y = 0;
    public int map_width = 4;
    public int map_height = 4;


    public float EulerX = 0;
    public float EulerY = 0;
    public float EulerZ = 0;

    public float rot_x_display = 0;
    public float rot_y_display = 0;


    void Start()
    {

        array2Da = new int[4, 4];
        array2Da[0, 0] = 0;
        array2Da[0, 1] = 1;
        array2Da[0, 2] = 1;
        array2Da[0, 3] = 0;
        array2Da[1, 0] = 0;
        array2Da[1, 1] = 2;
        array2Da[1, 2] = 0;
        array2Da[1, 3] = 0;
        array2Da[2, 0] = 1;
        array2Da[2, 1] = 1;
        array2Da[2, 2] = 0;
        array2Da[2, 3] = 0;
        array2Da[3, 0] = 0;
        array2Da[3, 1] = 1;
        array2Da[3, 2] = 1;
        array2Da[3, 3] = 0;


        //array2Da[0,0] = 1;
        //Debug.Log(array2Da[0,0].ToString());
        endPosition = GetComponent<Transform>().position;


        //Search player position
        for (int i = 0; i < map_width; i++)
        {
            for (int j = 0; j < map_height; j++)
            {
                if (array2Da[i, j] == 2)
                {
                    curr_x = i;
                    curr_y = j;
                    array2Da[i, j] = 1;
                }
            }
        }
    }

    void Update()
    {

        if(Input.GetMouseButton(2))
        {
            float rot_x = 0;
            float rot_y = 0;

            rot_x = Input.GetAxis("Mouse Y");
            rot_y = Input.GetAxis("Mouse X");
                
            if(rot_x != 0)
            {
                transform.rotation = Quaternion.Euler(transform.rotation.eulerAngles.x - rot_x, transform.rotation.eulerAngles.y, 0);
            }

            if (rot_y != 0)
            {
                transform.rotation = Quaternion.Euler(transform.rotation.eulerAngles.x, transform.rotation.eulerAngles.y + rot_y, 0);
            }

            EulerX = transform.localEulerAngles.x;
            EulerY = transform.localEulerAngles.y;
            EulerZ = transform.localEulerAngles.z;

            rot_x_display = rot_x;
            rot_y_display = rot_y;
        }

        GetComponent<Transform>().position = Vector3.MoveTowards(GetComponent<Transform>().position, endPosition, movSpeed * Time.deltaTime);
        if (GetComponent<Transform>().position == endPosition)
        {
            //2D coordinates, y=z in 3D coordinates
            int tile_mov_x = 0;
            int tile_mov_y = 0;

            if (Input.GetKeyDown(KeyCode.Q)) //Left
            {

                gameObject.transform.Rotate(Vector3.up, -90);
            }
            if (Input.GetKeyDown(KeyCode.E)) //Left
            {
                
                gameObject.transform.Rotate(Vector3.up, 90);

            }


            if (Input.GetKeyDown(KeyCode.A)) //Left
            {
                tile_mov_x = -1;
            }
            else if (Input.GetKeyDown(KeyCode.D)) //Right
            {
                tile_mov_x = 1;
            }
            else if (Input.GetKeyDown(KeyCode.W)) //Up
            {
                tile_mov_y = 1;
            }
            else if (Input.GetKeyDown(KeyCode.S)) //Down
            {
                tile_mov_y = -1;
            }

            //Calculate endPosition
            if ((tile_mov_x != 0 || tile_mov_y != 0) && array2Da[curr_x + tile_mov_x, curr_y + tile_mov_y] == 1)
            {
                endPosition = new Vector3(GetComponent<Transform>().position.x + distanceToMove * tile_mov_x, GetComponent<Transform>().position.y, GetComponent<Transform>().position.z + distanceToMove * tile_mov_y);
                curr_x += tile_mov_x;
                curr_y += tile_mov_y;
            }
        }
    }
}