using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class SwordParticles : CulverinBehaviour
{
    public GameObject sword_wall_particle;
    public GameObject sword_enemy_particle;

    CompParticleSystem wall_particle;
    CompParticleSystem enemy_particle;

    CompCollider col;

    void Start()
    {
        col = GetComponent<CompCollider>();

        sword_wall_particle = GetLinkedObject("sword_wall_particle");
        sword_enemy_particle = GetLinkedObject("sword_enemy_particle");

        wall_particle = sword_wall_particle.GetComponent<CompParticleSystem>();
        enemy_particle = sword_enemy_particle.GetComponent<CompParticleSystem>();
    }

    void OnContact()
    {
        Debug.Log("--- OnContact ---");
        col = GetComponent<CompCollider>();
        GameObject obj = col.GetCollidedObject();
        if (obj != null)
        {
            Debug.Log(obj.GetName());
            if (obj.CompareTag("Enemy"))
            {
                Debug.Log("[green] PAM");
                col = GetComponent<CompCollider>();
                Vector3 point = col.GetContactPoint();

                col = GetComponent<CompCollider>();
                Vector3 normal = col.GetContactNormal();

                sword_enemy_particle.GetComponent<Transform>().SetUpVector(normal);
                sword_enemy_particle.GetComponent<Transform>().SetPosition(point);

                enemy_particle = sword_enemy_particle.GetComponent<CompParticleSystem>();
                enemy_particle.ActivateEmission(true);

                Debug.Log("Enemy particle");
            }
            else if (obj.CompareTag("undefined"))
            {
                Debug.Log("[yellow] PUM");
                col = GetComponent<CompCollider>();
                Vector3 point = col.GetContactPoint();

                col = GetComponent<CompCollider>();
                Vector3 normal = col.GetContactNormal();

                sword_wall_particle.GetComponent<Transform>().SetUpVector(normal);
                sword_wall_particle.GetComponent<Transform>().SetPosition(point);

                wall_particle = sword_wall_particle.GetComponent<CompParticleSystem>();
                wall_particle.ActivateEmission(true);

                Debug.Log("Wall particle");

            }
        }
    }

}

