using CulverinEditor;
using CulverinEditor.Debug;


public class Projectile : CulverinBehaviour
{
    public float damage = 10f;
    public Vector3 speed = Vector3.Zero;
    public float speed_projectil = 1.0f;
    public bool collision;
    CompRigidBody rb;

    private bool destroyed = false;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        Shoot();
        collision = true;
     
    }

    public void Shoot()
    {
        rb.ApplyImpulse(speed * speed_projectil);
    }

    void Update()
    {

    }

    void OnContact()
    {
        Debug.Log("CONTACT", Department.STAGE, Color.RED);
        CompCollider col = GetComponent<CompCollider>();
        GameObject collided_obj = col.GetCollidedObject();
       
        
       
        if (collided_obj != null && destroyed == false)
        {
            if(collided_obj.CompareTag("player"))
            {
                CharactersManager cm = collided_obj.GetComponent<CharactersManager>();

                if (cm != null)
                {
                    cm.GetDamage(damage);
                    StatsScore.PuzzleDamage();
                }
            }

            Debug.Log(collided_obj.GetTag().ToString(), Department.STAGE) ;

        }
        else Debug.Log("Collided obj NULL", Department.STAGE, Color.ORANGE);

        if (!destroyed)
        {
            destroyed = true;
            col.CollisionActive(false);
            Destroy(gameObject);
        }
    }
}

