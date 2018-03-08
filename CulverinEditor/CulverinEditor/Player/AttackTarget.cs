using CulverinEditor;
using CulverinEditor.Debug;

public class AttackTarget : CulverinBehaviour
{
    CompRigidBody dynamic_collider = null;
    //WeaponController weapon_controller = null;
    //WeaponController weapon_controller = null; // FIX THIS

    bool expecting_collision = false;

    void Start()
    {

    }

    void Update()
    {
        if (expecting_collision == true)
        {
            ResetColliderPosition();
            expecting_collision = false;
        }
    }

    void OnTriggerEnter()
    {
        //GameObject target = GetComponent<CompCollider>().GetCollidedObject();
        //target = GetLinkedObject("target");

        //weapon_controller = GetComponent<LeftWeapon>();
        //weapon_controller.enemy_obj = target;
        //weapon_controller = GetComponent<LeftWeapon>();
        //weapon_controller.AttackHit();
    }

    public void CheckAttackTarget()
    {
        Debug.Log("CheckAttackTarget");
        dynamic_collider = GetComponent<CompRigidBody>();

        Vector3 position = dynamic_collider.GetColliderPosition();
        position += transform.forward;
        Quaternion rotation = dynamic_collider.GetColliderQuaternion();
        dynamic_collider.MoveKinematic(position, rotation);
        expecting_collision = true;
    }

    void ResetColliderPosition()
    {
        dynamic_collider = GetComponent<CompRigidBody>();

        Vector3 position = dynamic_collider.GetColliderPosition();
        position -= transform.forward;
        Quaternion rotation = dynamic_collider.GetColliderQuaternion();
        dynamic_collider.MoveKinematic(position, rotation);

    }
}
