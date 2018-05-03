using CulverinEditor;
using CulverinEditor.Debug;


public class Fireball : CulverinBehaviour
{
    public float damage = 25;
    public float life = 5.0f;
    public Vector3 vfront = Vector3.Zero;
    public GameObject fireball_particles;
    public GameObject fireball_particles2;
    public GameObject fireball_particles3;
    public GameObject fireball_particles4;

    private CompRigidBody rb;
    public float vel = 120;
    private float timer = 0.0f;
    public bool rotate = true;
    public bool fireball = true;

    private bool destroyed = false;

    private float final_pos = 0.0f;
    private int distance = 0;

    public void SetDamage(float set_damage)
    {
        damage = set_damage;
    }

    void Start()
    {
        Debug.Log("Vfront" + vfront, Department.PLAYER, Color.ORANGE);
        rb = GetComponent<CompRigidBody>();
        Shoot();
        timer = 0.0f;

        GetComponent<CompAudio>().PlayEvent("DaenerysFireballImpact");

        if (fireball == false)
        {
            if (vfront.x >= 2)
            {
                final_pos = transform.GetPosition().x + 25.4f * distance;
            }
            else if (vfront.x <= -2)
            {
                final_pos = transform.GetPosition().x - 25.4f * distance;
            }
            else if (vfront.z >= 2)
            {
                final_pos = transform.GetPosition().z + 25.4f * distance;
            }
            else if (vfront.z <= -2)
            {
                final_pos = transform.GetPosition().z - 25.4f * distance;
            }
        }

    }

    public void Shoot()
    {
        rb.ApplyImpulse(vfront * vel); // Forward impulse
        rb.ApplyTorqueForce(new Vector3(0, 0, 40));

    }

    void Update()
    {
        if (fireball == false)
        {
            if (vfront.x >= 2 && transform.GetPosition().x > final_pos)
            {
                Destroy(gameObject);
            }
            else if (vfront.x <= -2 && transform.GetPosition().x < final_pos)
            {
                Destroy(gameObject);
            }
            else if (vfront.z >= 2 && transform.GetPosition().z > final_pos)
            {
                Destroy(gameObject);
            }
            else if (vfront.z <= -2 && transform.GetPosition().z < final_pos)
            {
                Destroy(gameObject);
            }
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

            if (fireball_particles2 != null)
            {
                fireball_particles_script = fireball_particles2.GetComponent<CompParticleSystem>();
                fireball_particles_script.ActivateEmission(true);
            }
            if (fireball_particles3 != null)
            {
                fireball_particles_script = fireball_particles3.GetComponent<CompParticleSystem>();
                fireball_particles_script.ActivateEmission(true);
            }
            if (fireball_particles4 != null)
            {
                fireball_particles_script = fireball_particles4.GetComponent<CompParticleSystem>();
                fireball_particles_script.ActivateEmission(true);
            }
        }

        GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();
        GetComponent<CompAudio>().PlayEvent("DaenerysFireballImpact");       
        // DAMAGE ---
        if (collided_obj != null)
        {
            // Check the specific enemy in front of you and apply dmg or call object OnContact
            EnemiesManager enemy_manager = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>();
            if (!destroyed)
            {
                if (enemy_manager.IsEnemy(collided_obj))
                {
                    enemy_manager.ApplyDamage(collided_obj, damage, Enemy_BT.ENEMY_GET_DAMAGE_TYPE.FIREBALL);
                }
                else
                {
                    destroyed = true;
                    Debug.Log("Destroy 1", Department.GENERAL);
                    Destroy(gameObject);
                    return;
                }
            }
        }


        if (fireball && destroyed == false)
        {
            col.CollisionActive(false);
            Debug.Log("Destroy 2", Department.GENERAL);
            Destroy(gameObject);
            destroyed = true;
            return;
        }
    }

    public void SetDistance(int distance)
    {
        this.distance = distance;
    }
}