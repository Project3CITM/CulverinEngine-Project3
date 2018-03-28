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

        enemy_particle = enemy_particle_obj.GetComponent<CompParticleSystem>();
        wall_particle = wall_particle_obj.GetComponent<CompParticleSystem>();

    }

    void OnContact()
    {
        GameObject obj = col.GetCollidedObject();

        if(obj.CompareTag("enemy"))
        {
            enemy_particle.ActivateEmission(true);
           
            //TODO: SET ORIENTATION
        }
        else if (obj.CompareTag("wall"))
        {
            wall_particle.ActivateEmission(true);
            //TODO: SET ORIENTATION
        }
    }

}

