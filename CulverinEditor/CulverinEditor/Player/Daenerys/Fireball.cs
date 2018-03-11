using CulverinEditor;
using CulverinEditor.Debug;


public class Fireball : CulverinBehaviour
{
    public float damage = 50;
    Vector3 speed;
    Vector3 gravity;
    bool collision;
    CompRigidBody rb;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        Shoot();
        damage = 10.0f;
    }

    public void Shoot()
    {
        Vector3 force = new Vector3(0, 5, 0);
        rb.ApplyImpulse(force + transform.forward * 50); // Forward impulse
        rb.ApplyTorqueForce(new Vector3(0, 0, 40)); // Fall force
    }

    void Update()
    {
        if (GetComponent<Transform>().local_position.y < -5)
        {
            Destroy(gameObject);
        }

        //if(rb.LockedTransform())
        //{
        //    Destroy(float delay)
        //}
    }

    void OnTriggerEnter()
    {
        GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();
        // DAMAGE ---
        if (collided_obj != null)
        {
            // Check the specific enemy in front of you and apply dmg or call object OnContact
            EnemiesManager enemy_manager = GetLinkedObject("enemies_obj").GetComponent<EnemiesManager>();
            if (enemy_manager.IsEnemy(collided_obj))
            {
                enemy_manager.ApplyDamage(collided_obj, damage);
            }
            else
            {
                CompCollider obj_col = collided_obj.GetComponent<CompCollider>();
                if (obj_col != null)
                {
                    obj_col.CallOnContact();
                }
            }
        }
        Destroy(gameObject);
    }

    void OnContact()
    {
        GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();
        // DAMAGE ---

        CompCollider obj_col = collided_obj.GetComponent<CompCollider>();
        if (obj_col != null)
        {
            obj_col.CallOnContact();
        }

        rb.LockTransform();
        Destroy(gameObject);
    }
}