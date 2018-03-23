using CulverinEditor;
using CulverinEditor.Debug;

public class RespawnWhenHit : CulverinBehaviour
{
    public int respawn_tile_x = 0;
    public int respawn_tile_y = 0;
    public float tile_size = 25.4f;
    public float respawn_height = 0.0f;


    void OnTriggerLost()
    {
        Debug.Log("COLLISION");
        CompCollider col = GetComponent<CompCollider>();

        if(col == null)
        {
            Debug.Log("WARNING: Object doesn't have collider component!");
            return;
        }


        GameObject obj = col.GetCollidedObject();
        if(obj != null)
        {
            Debug.Log("Obj Fount");
        }
        Debug.Log(obj.GetName());
        Debug.Log(obj.GetTag().ToString());
        if(obj.CompareTag("player"))
        {
            Debug.Log("Player");
            Vector3 respawn_pos_float3 = new Vector3(respawn_tile_x * tile_size, respawn_height, respawn_tile_y * tile_size);
            obj.transform.SetPosition(respawn_pos_float3);
            CompRigidBody collbody = obj.GetComponent<CompRigidBody>();
            collbody.ResetForce();
            collbody.LockTransform();
            MovementController controller = obj.GetComponent<MovementController>();
            controller.endPosition = respawn_pos_float3;
            controller.curr_x = respawn_tile_x;
            controller.curr_y = respawn_tile_y;
            controller.moving = false;
        }

    }


}