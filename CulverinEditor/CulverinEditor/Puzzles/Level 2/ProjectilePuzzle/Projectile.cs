using CulverinEditor;
using CulverinEditor.Debug;


public class Projectile : CulverinBehaviour
{
    public float damage = 10f;
    public Vector3 speed = Vector3.Zero;
    public bool collision;
    CompRigidBody rb;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        Shoot();
        collision = true;
     
    }

    public void Shoot()
    {
        rb.ApplyImpulse(speed * 100);
    }

    void Update()
    {

    }

    void OnContact()
    {
        Debug.Log("CONTACT", Department.STAGE, Color.RED);
        CompCollider col = GetComponent<CompCollider>();
        GameObject collided_obj = col.GetCollidedObject();
       
        
       
        if (collided_obj != null)
        {
            if(collided_obj.CompareTag("player"))
            {
                CharactersManager cm = collided_obj.GetComponent<CharactersManager>();

                if (cm != null)
                {
                    cm.GetDamage(damage);
                }

                Destroy(gameObject);
            }
            if(collided_obj.CompareTag("obstacle"))
            {
                Destroy(gameObject);
            }

            Debug.Log(collided_obj.GetTag().ToString(), Department.STAGE) ;

        }
        else Debug.Log("Collided obj NULL", Department.STAGE, Color.ORANGE);

    }
}

