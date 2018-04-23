using CulverinEditor;
using CulverinEditor.Debug;

public class TestParticle : CulverinBehaviour
{
    CompParticleSystem particle;

    void Start()
    {
        particle = GetComponent<CompParticleSystem>();
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.E))
        {
          particle = GetComponent<CompParticleSystem>();
            particle.ActivateEmission(true);
        }
    }
}