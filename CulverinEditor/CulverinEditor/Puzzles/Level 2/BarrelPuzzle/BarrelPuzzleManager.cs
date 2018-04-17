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
        if (Input.GetKeyDown(KeyCode.B))
            must_move = !must_move;

        if (must_move)
            MoveWater();
    }

    // -----------------------------------------------------

    void MoveWater()
    {
        Transform water_trans = water_plane.GetComponent<Transform>();

        if (water_trans == null)
            Debug.Log("[error] Could not get water transform.");

        if (water_destination_pos.Distance(water_trans.local_position, water_destination_pos) <= 0.2f) // Just let a small threshold on distance
        {
            must_move = false;
            water_trans.SetPosition(water_destination_pos);
        }
        else
        {
            Vector3 pos = water_trans.local_position;
            pos += (Vector3.Up * water_speed * Time.deltaTime);
            water_trans.SetPosition(pos);
        }
    }

    public void OnPuzzleActivated()
    {
        must_move = true;
    }
}