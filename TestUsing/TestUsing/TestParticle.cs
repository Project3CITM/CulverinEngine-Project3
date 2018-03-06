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
        particle = GetComponent<CompParticleSystem>();
        if (Input.GetKeyDown(KeyCode.E))
        {
            particle.ActivateEmission(true);
        }
    }
}