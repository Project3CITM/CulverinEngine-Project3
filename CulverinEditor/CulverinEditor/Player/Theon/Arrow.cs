using CulverinEditor;
using CulverinEditor.Debug;


public class Arrow : CulverinBehaviour
{
    float damage;
    Vector3 speed;
    Vector3 gravity;
    bool collision;
    CompRigidBody rb;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        Shoot();
    }

    public void Shoot()
    {
        rb.UnLockTransform();
        rb.ApplyImpulse(new Vector3(0,10,10)); // Forward impulse
        rb.ApplyTorqueForce(new Vector3(0, 0, 0)); // Fall force
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
       
        
        // DAMAGE ---

        // DESTROY ---
        if(collision)
        {
            //Destroy();
        }
    }

    void OnContact()
    {
        //rb.LockTransform();
    }
}

