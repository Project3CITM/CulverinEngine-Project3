using CulverinEditor;
using CulverinEditor.Debug;

public class AttackTarget : CulverinBehaviour
{
    CompRigidBody dynamic_collider = null;
    WeaponController weapon_controller = null;

    bool expecting_collision = false;

    void Start()
    {
        dynamic_collider = GetComponent<CompRigidBody>();
        weapon_controller = GetComponent<WeaponController>();
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
        GameObject target = GetComponent<CompCollider>().GetCollidedObject();
        if (target != null && weapon_controller != null)
        {
            weapon_controller.enemy = target.GetComponent<EnemyController>();
            weapon_controller.AttackHit();
        }
    }

    public void CheckAttackTarget()
    {
        if (dynamic_collider != null)
        {
            Vector3 position = dynamic_collider.GetColliderPosition();
            position += transform.forward;
            Quaternion rotation = dynamic_collider.GetColliderQuaternion();

            dynamic_collider.MoveKinematic(position, rotation);
            expecting_collision = true;
        }
    }

    void ResetColliderPosition()
    {
        if (dynamic_collider != null)
        {
            Vector3 position = dynamic_collider.GetColliderPosition();
            position -= transform.forward;
            Quaternion rotation = dynamic_collider.GetColliderQuaternion();

            dynamic_collider.MoveKinematic(position, rotation);
        }
    }
}
