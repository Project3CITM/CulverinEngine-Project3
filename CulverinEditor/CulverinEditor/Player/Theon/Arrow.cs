using CulverinEditor;
using CulverinEditor.Debug;


public class Arrow : CulverinBehaviour
{
    public float damage = 20;
    public Vector3 speed = Vector3.Zero;
    public bool collision;
    CompRigidBody rb;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        Shoot();
        collision = true;
        damage = 10.0f;
    }

    public void Shoot()
    {
        rb.UnLockTransform();
        Vector3 force = new Vector3(0, 5, 0);
        rb.ApplyImpulse(force + speed*100); // Forward impulse
        rb.ApplyTorqueForce(speed*40); // Fall force
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
        //Debug.Log("Trigger Start");
        //GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();
        //// DAMAGE ---
        //Debug.Log("Collided");

        //if (collided_obj != null)
        //{
        //    Debug.Log("OnTriggerEnter");
        //    Debug.Log(collided_obj.GetName());
        //    // Check the specific enemy in front of you and apply dmg or call object OnContact
        //    EnemiesManager enemy_manager = GetLinkedObject("enemies_obj").GetComponent<EnemiesManager>();
        //    if (enemy_manager.IsEnemy(collided_obj))
        //    {
        //        enemy_manager.ApplyDamage(collided_obj, damage);
        //    }
        //    else
        //    {
        //        CompCollider obj_col = collided_obj.GetComponent<CompCollider>();
        //        if (obj_col != null)
        //        {
        //            obj_col.CallOnContact();
        //        }
        //    }
        //}

        //// DESTROY ---
        //if (collision)
        //{
        //    Destroy(gameObject);
        //}
    }

    void OnContact()
    {
        Debug.Log("Trigger Start");
        GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();
        // DAMAGE ---
        Debug.Log("Collided");

        if (collided_obj != null)
        {
            Debug.Log("OnTriggerEnter");
            Debug.Log(collided_obj.GetName());
            // Check the specific enemy in front of you and apply dmg or call object OnContact
            EnemiesManager enemy_manager = GetLinkedObject("enemies_obj").GetComponent<EnemiesManager>();
            if (enemy_manager.IsEnemy(collided_obj))
            {
                enemy_manager.ApplyDamage(collided_obj, damage);
                Destroy(gameObject);
            }
            else
            {
                CompCollider obj_col = collided_obj.GetComponent<CompCollider>();
                if (obj_col != null)
                {
                    obj_col.CallOnContact();
                }
                Destroy(gameObject);
            }
        }
        else
        {
            Debug.Log("destroy 1");
            Destroy(gameObject);
            Debug.Log("destroy 2");
        }
    }
}

