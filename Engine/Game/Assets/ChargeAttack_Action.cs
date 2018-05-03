using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;

public class ChargeAttack_Action : Action
{
    PathNode objective;
    Vector3 position;
    float movement_x;
    float movement_z;
    float speed_x;
    float speed_z;
    public float speed;
    public float after_cd;
    float timer;
    int player_x;
    int player_y;
    GameObject player;

    public ChargeAttack_Action()
    {
        action_type = ACTION_TYPE.CHARGE_ATTACK_ACTION;
    }

    void Start()
    {
        objective = new PathNode(0,0);
        movement_z = 0.0f;
        movement_x = 0.0f;
        speed_x = 0.0f;
        speed_z = 0.0f;
        speed = 1.0f;
        after_cd = 1.0f;
        timer = 0.0f;
    }

    public override bool ActionStart()
    {
        player = GetLinkedObject("player_obj");
        player.GetComponent<MovementController>().GetPlayerPos(out player_x, out player_y);
        float player_transform_x = player.GetComponent<Transform>().position.x;
        float player_transform_z = player.GetComponent<Transform>().position.z;
        objective = new PathNode(player_x, player_y);

        position = GetComponent<Transform>().position;
        movement_x = player_transform_x - position.x;
        movement_z = player_transform_z - position.z;

        float x_factor = movement_x / (movement_x + movement_z);
        float z_factor = movement_z / (movement_x + movement_z);

        speed_x = speed * x_factor;
        speed_z = speed * z_factor;

        //Set Anim Duration
        float duation = GetDuration();

        return true;
    }

    // Update is called once per frame
    public override ACTION_RESULT ActionUpdate()
    {
        position.x += speed_x * Time.deltaTime;
        position.z += speed_z * Time.deltaTime;

        player.GetComponent<MovementController>().GetPlayerPos(out player_x, out player_y);
        float tile_size = GetComponent<Movement_Action>().tile_size;
        uint current_x = (uint) (position.x / tile_size);
        uint current_y = (uint) (position.z / tile_size);

        if (current_x == player_x && current_y == player_y)
        {
            //push player
        }

        if (movement_z == 0.0f && movement_x == 0.0f)
        {
            GetComponent<Movement_Action>().tile.SetCoords(objective.GetTileX(), objective.GetTileY());
            return ACTION_RESULT.AR_SUCCESS;
        }

        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    float GetDuration()
    {
        Vector3 distance = new Vector3(player.transform.position - position);
        return distance.Length / speed;
    }
}
