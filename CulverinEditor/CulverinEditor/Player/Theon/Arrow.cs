using CulverinEditor;
using CulverinEditor.Debug;


public class Arrow : CulverinBehaviour
{
    float damage = 10;
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
        rb.UnLockTransform();
        Vector3 force = new Vector3(0, 10, 0);
        rb.ApplyImpulse(force + transform.forward*50); // Forward impulse
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
        Enemy_BT obj = collided_obj.GetComponent<Enemy_BT>();
        if(obj != null)
        {
            obj.ApplyDamage(damage);
        }
        else
        {
            CompCollider obj_col = collided_obj.GetComponent<CompCollider>();
            if(obj_col != null)
            {
                obj_col.CallOnContact();
            }
        }

        // DESTROY ---
        if(collision)
        {
            Destroy(gameObject);
        }
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

