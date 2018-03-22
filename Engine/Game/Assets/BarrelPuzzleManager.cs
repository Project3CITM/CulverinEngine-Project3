using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelPuzzleManager : CulverinBehaviour
{
    // Reference to the water GO
    public GameObject water_plane;

    // How much the water must move
    public float move_distance = 5.0f;
    // Speed of the water moving up
    public float water_speed = 2.0f;


    
    private bool must_move = false;

    private Vector3 water_initial_pos;
    private Vector3 water_destination_pos;


    // -----------------------------------------------------

    void Start()
    {
        water_plane = GetLinkedObject("water_plane");
        water_initial_pos = water_plane.transform.local_position;
        water_destination_pos = water_initial_pos + (Vector3.Up * move_distance);
    }

    void Update()
    {
        // TMP: Debugging purposes.
<<<<<<< HEAD
        if (Input.GetKeyDown(KeyCode.Space))
        {
            must_move = !must_move;
            Debug.Log("Space pressed " + ((must_move) ? "started" : "stopped") + " moving.");
            Debug.Log("Destination: " + water_destination_pos.x + ", " + water_destination_pos.y + ", " + water_destination_pos.z);
        }

        if (must_move)
        {
            Debug.Log("Moving---------------");
=======
        //if (Input.GetKeyDown(KeyCode.Space))
        //{
        //    must_move = !must_move;
        //    Debug.Log("Space pressed " + ((must_move) ? "started" : "stopped") + " moving.");
        //    Debug.Log("Destination: " + water_destination_pos.x + ", " + water_destination_pos.y + ", " + water_destination_pos.z);
        //}

        if (must_move)
        {
            //Debug.Log("Moving---------------");
>>>>>>> Stage
            MoveWater();
        }
    }

    // -----------------------------------------------------

    void MoveWater()
    {
        Transform water_trans = water_plane.GetComponent<Transform>();

        if (water_trans == null)
        {
            Debug.Log("ALERT: Could not get water transform.");
        }

<<<<<<< HEAD
        Debug.Log("Water distance to destination: " + water_destination_pos.Distance(water_trans.local_position, water_destination_pos));
=======
        //Debug.Log("Water distance to destination: " + water_destination_pos.Distance(water_trans.local_position, water_destination_pos));
>>>>>>> Stage

        if (water_destination_pos.Distance(water_trans.local_position, water_destination_pos) <= 0.2f) // Just let a small threshold on distance
        {
            must_move = false;
            water_trans.SetPosition(water_destination_pos);
            Debug.Log("Water reached the destination.");
        }
        else
        {
            //Debug.Log("Moving---------------");

            Vector3 pos = water_trans.local_position;

<<<<<<< HEAD
            Debug.Log("Water pos: " + pos.x + ", " + pos.y + ", " + pos.z);
=======
            //Debug.Log("Water pos: " + pos.x + ", " + pos.y + ", " + pos.z);
>>>>>>> Stage

            pos += (Vector3.Up * water_speed * Time.deltaTime);
            water_trans.SetPosition(pos);

<<<<<<< HEAD
            Debug.Log("Water final pos: " + pos.x + ", " + pos.y + ", " + pos.z);
        }
    }
=======
            //Debug.Log("Water final pos: " + pos.x + ", " + pos.y + ", " + pos.z);
        }
    }

    public void OnPuzzleActivated()
    {
        Debug.Log("Water started moving.");
        must_move = true;
    }
>>>>>>> Stage
}