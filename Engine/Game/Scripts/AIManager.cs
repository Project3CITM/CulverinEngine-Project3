using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;
using CulverinEditor.Map;
using System.Collections.Generic;
using System.Collections;

public class AIManager : CulverinBehaviour
{
    public enum MYDIRECTION
    {
        NO_DIR = -1,
        NORTH = 0,
        SOUTH = 180,
        EAST = 270,
        WEST = 90

    }

    public enum MYSTATE
    {
        IDLE,
        MOVE_TO_PLAYER,
        ATTACK
    }

    //bool path_reached = false;
    Pathfinder path_creator;
    List<PathNode> my_path;

    bool path_reached;

    public int x, y;
    int my_dir;
    float tile_size;
    public float movement_speed, rotation_speed;
    MYSTATE state;

    Transform my_trans;

    // Use this for initialization
    void Start()
    {

        //Set Variables
        path_creator = new Pathfinder();
        path_reached = false;
        state = MYSTATE.IDLE;
	my_trans = GetComponent<Transform>();

        //Init Pathfinder 
        path_creator.Init();

        //Debug
        my_path = path_creator.CalculatePath(new PathNode(x, y), new PathNode(0, 5));


    }

    void Update()
    {
        if (!path_reached)
        {

            MoveNextTile();


        }

        switch (state)
        {
            case MYSTATE.IDLE:

                break;

            case MYSTATE.MOVE_TO_PLAYER:

                break;

            case MYSTATE.ATTACK:

                break;
        }


    }

    void MoveNextTile()
    {

        if (my_path.Count == 0)
        {
            path_reached = true;
            return;
        }

        MYDIRECTION new_dir = CheckRotation(my_path[0].GetTileX(), my_path[0].GetTileY());

        if (my_dir == (int)new_dir || new_dir == MYDIRECTION.NO_DIR)
        {
		if(my_dir == (int)MYDIRECTION.EAST || my_dir == (int)MYDIRECTION.WEST)
			my_trans.local_position += GetComponent<Transform>().GetForwardVector() * -movement_speed * Time.DeltaTime();
		else    my_trans.local_position += GetComponent<Transform>().GetForwardVector() * movement_speed * Time.DeltaTime();
		
            if (ReachedTile())
            {
                x = my_path[0].GetTileX();
                y = my_path[0].GetTileY();
                my_path.Remove(my_path[0]);
            }


        }
        else
        {
            UpdateRotation();

            Rotate(new_dir);
        }


    }

    void ChangePath()
    {
        my_path.Clear();
        my_path = path_creator.CalculatePath(new PathNode(x, y), new PathNode(0, 0));

    }

    void Rotate(MYDIRECTION dir_to_go)
    {
        switch (my_dir)
        {
            case (int)MYDIRECTION.NORTH:

                if (dir_to_go == MYDIRECTION.EAST || dir_to_go == MYDIRECTION.SOUTH)
                {
                    if (my_dir != (int)MYDIRECTION.EAST)
                        RotateAngle(-rotation_speed * Time.DeltaTime());

                    return;
                }

                if (dir_to_go == MYDIRECTION.WEST)
                {
                    if (my_dir != (int)MYDIRECTION.WEST)
                        RotateAngle(rotation_speed * Time.DeltaTime());

                    return;
                }

                break;

            case (int)MYDIRECTION.SOUTH:

                if (dir_to_go == MYDIRECTION.EAST || dir_to_go == MYDIRECTION.NORTH)
                {
                    if (my_dir != (int)MYDIRECTION.EAST)
                        RotateAngle(rotation_speed * Time.DeltaTime());

                    return;
                }

                if (dir_to_go == MYDIRECTION.WEST)
                {
                    if (my_dir != (int)MYDIRECTION.WEST)
                        RotateAngle(-rotation_speed * Time.DeltaTime());

                    return;
                }

                break;

            case (int)MYDIRECTION.EAST:

                if (dir_to_go == MYDIRECTION.NORTH || dir_to_go == MYDIRECTION.WEST)
                {
                    if (my_dir != (int)MYDIRECTION.NORTH)
                        RotateAngle(rotation_speed * Time.DeltaTime());
                    return;
                }

                if (dir_to_go == MYDIRECTION.SOUTH)
                {
                    if (my_dir != (int)MYDIRECTION.SOUTH)
                        RotateAngle(-rotation_speed * Time.DeltaTime());

                    return;
                }

                break;

            case (int)MYDIRECTION.WEST:

                if (dir_to_go == MYDIRECTION.NORTH || dir_to_go == MYDIRECTION.EAST)
                {
                    if (my_dir != (int)MYDIRECTION.NORTH)
                        RotateAngle(-rotation_speed * Time.DeltaTime());

                    return;
                }

                if (dir_to_go == MYDIRECTION.SOUTH)
                {
                    if (my_dir != (int)MYDIRECTION.SOUTH)
                        RotateAngle(rotation_speed * Time.DeltaTime());

                    return;
                }

                break;
        }
    }

    void UpdateRotation()
    {
        float y_rotation = (int)my_trans.GetRotation().y;

        if (y_rotation > 360)
            y_rotation -= 360;
        else
        {
            if (y_rotation < 0)
                y_rotation += 360;
        }

        if (y_rotation >= -1 && y_rotation <= 1)
        {
            my_dir = (int)MYDIRECTION.NORTH;
            return;
        }

        if (y_rotation >= 269 && y_rotation <= 271)
        {
            my_dir = (int)MYDIRECTION.EAST;
            return;
        }

        if (y_rotation >= 179 && y_rotation <= 181)
        {
            my_dir = (int)MYDIRECTION.SOUTH;
            return;
        }

        if (y_rotation >= 89 && y_rotation <= 91)
        {
            my_dir = (int)MYDIRECTION.WEST;
            return;
        }
    }

    void RotateAngle(float angle_in_deg)
    {
        my_trans.RotateAroundAxis(new Vector3(0, 1, 0), angle_in_deg);
    }

    public MYDIRECTION CheckRotation(int tile_to_go_x, int tile_to_go_y)
    {
        int west_east = x - tile_to_go_x;
        int north_south = y - tile_to_go_y;

        if (west_east > 0)
            return MYDIRECTION.WEST;

        if (west_east < 0)
            return MYDIRECTION.EAST;

        if (north_south > 0)
            return MYDIRECTION.NORTH;

        if (north_south < 0)
            return MYDIRECTION.SOUTH;

        return MYDIRECTION.NO_DIR;
    }

    bool ReachedTile()
    {
        Vector3 my_pos = my_trans.local_position;

        switch (my_dir)
        {
            case (int)MYDIRECTION.NORTH:

                if (my_pos.z <= (my_path[0].GetTileY() * tile_size))
                    return true;

                break;

            case (int)MYDIRECTION.SOUTH:

                if (my_pos.z >= (my_path[0].GetTileY() * tile_size))
                    return true;

                break;

            case (int)MYDIRECTION.EAST:

                if (my_pos.x >= (my_path[0].GetTileX() * tile_size))
                    return true;

                break;

            case (int)MYDIRECTION.WEST:

                if (my_pos.x <= (my_path[0].GetTileX() * tile_size))
                    return true;

                break;
        }
        return false;

    }
}