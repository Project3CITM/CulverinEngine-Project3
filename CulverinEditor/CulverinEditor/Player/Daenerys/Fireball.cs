using CulverinEditor;
using CulverinEditor.Debug;


public class Fireball : CulverinBehaviour
{
    public float damage = 50;
    public float life = 5.0f;
    public Vector3 vfront = Vector3.Zero;
    private CompRigidBody rb;
    private float timer = 0.0f;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        Shoot();
        damage = 10.0f;
        timer = 0.0f;
        life = 5.0f;
    }

    public void Shoot()
    {
        rb.ApplyImpulse(vfront * 50); // Forward impulse
        rb.ApplyTorqueForce(new Vector3(0, 0, 40)); // Fall force
    }

    void Update()
    {
        timer += Time.deltaTime;

        if(timer >= life)
        {
            Destroy(gameObject);
        }
    }

    void OnTriggerEnter()
    {
        //GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();
        //// DAMAGE ---
        //if (collided_obj != null)
        //{
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
        Destroy(gameObject);
    }

    void OnContact()
    {
        GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();
        // DAMAGE ---
        if (collided_obj != null)
        {
            // Check the specific enemy in front of you and apply dmg or call object OnContact
            EnemiesManager enemy_manager = GetLinkedObject("enemies_obj").GetComponent<EnemiesManager>();
            if (enemy_manager.IsEnemy(collided_obj))
            {
                //enemy_manager.ApplyDamage(collided_obj, damage);
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