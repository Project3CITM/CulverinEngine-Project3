using CulverinEditor;
using CulverinEditor.Debug;


public class Fireball : CulverinBehaviour
{
    public float damage = 50;
    public float life = 5.0f;
    public Vector3 vfront = Vector3.Zero;
    public GameObject fireball_particles;
    private CompRigidBody rb;
    public float vel = 50;
    private float timer = 0.0f;
    public bool rotate = true;
    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        if(vfront == Vector3.Zero)
        {
            vfront = transform.GetBackwardVector();
        }
        Shoot();
        damage = 10.0f;
        timer = 0.0f;
        life = 5.0f;
        
        //GetComponent<CompAudio>().PlayEvent("DaenerysFire"); //Change This!!
    }

    public void Shoot()
    {
        rb.ApplyImpulse(vfront * vel); // Forward impulse
        if(rotate)
        {
            rb.ApplyTorqueForce(new Vector3(0, 0, 40));
        }
    }

    void Update()
    {
        timer += Time.deltaTime;

        if(timer >= life)
        {
            Destroy(gameObject);
        }
    }

    void OnContact()
    {
        CompCollider col = GetComponent<CompCollider>();
        if (fireball_particles != null)
        {
            Vector3 point = col.GetContactPoint();
            Vector3 normal = col.GetContactNormal();

            fireball_particles.GetComponent<Transform>().SetUpVector(normal);
            point = point + normal * 2;
            fireball_particles.GetComponent<Transform>().SetPosition(point);

            CompParticleSystem fireball_particles_script = fireball_particles.GetComponent<CompParticleSystem>();
            fireball_particles_script.ActivateEmission(true);

            //Iterate all childs, they have a ParticleSystem too
            int childs = fireball_particles.ChildCount();
            for (int i = 0; i < childs; i++) 
            {
                fireball_particles.GetChildByIndex(i).GetComponent<CompParticleSystem>().ActivateEmission(true);
            }
        }


        GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();
        // DAMAGE ---
        if (collided_obj != null)
        {
            rb = GetComponent<CompRigidBody>();
            //Lock transform to avoid trespassing more than one collider
            rb.LockTransform();

            // Check the specific enemy in front of you and apply dmg or call object OnContact
            EnemiesManager enemy_manager = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>();
            if (enemy_manager.IsEnemy(collided_obj))
            {
                enemy_manager.ApplyDamage(collided_obj, damage, Enemy_BT.ENEMY_GET_DAMAGE_TYPE.FIREBALL);
            }
        }

        GetComponent<CompAudio>().PlayEvent("DaenerysFireballImpact");
        GetComponent<CompAudio>().StopEvent("DaenerysFire");      

        Destroy(gameObject);
    }
}