using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class SetParticleTransf : CulverinBehaviour
{
    public GameObject wall_particle_obj;
    public GameObject enemy_particle_obj;

    CompParticleSystem wall_particle;
    CompParticleSystem enemy_particle;


    CompCollider col;

    void Start()
    {
        col = GetComponent<CompCollider>();

        enemy_particle_obj = GetLinkedObject("enemy_particle_obj");
        wall_particle_obj = GetLinkedObject("wall_particle_obj");

        enemy_particle = enemy_particle_obj.GetComponent<CompParticleSystem>();
        wall_particle = wall_particle_obj.GetComponent<CompParticleSystem>();

    }

    void OnContact()
    {
        col = GetComponent<CompCollider>();
        GameObject obj = col.GetCollidedObject();
        if (obj != null)
        {
            Debug.Log(obj.GetName());
            if (obj.CompareTag("enemy"))
            {
                col = GetComponent<CompCollider>();
                Vector3 point = col.GetContactPoint();

                col = GetComponent<CompCollider>();
                Vector3 normal = col.GetContactNormal();

                enemy_particle_obj.GetComponent<Transform>().SetUpVector(normal);
                enemy_particle_obj.GetComponent<Transform>().SetPosition(point);

                enemy_particle = enemy_particle_obj.GetComponent<CompParticleSystem>();
                enemy_particle.ActivateEmission(true);
            }
            else if (obj.CompareTag("obstacle"))
            {
                col = GetComponent<CompCollider>();
                Vector3 point = col.GetContactPoint();

                col = GetComponent<CompCollider>();
                Vector3 normal = col.GetContactNormal();

                wall_particle_obj.GetComponent<Transform>().SetUpVector(normal);
                wall_particle_obj.GetComponent<Transform>().SetPosition(point);

                wall_particle = wall_particle_obj.GetComponent<CompParticleSystem>();
                wall_particle.ActivateEmission(true);
            }
        }
    }

}

