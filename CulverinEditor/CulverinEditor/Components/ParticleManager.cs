using CulverinEditor;
using CulverinEditor.Debug;

public class ParticleManager : CulverinBehaviour
{
    public float duration = 5f;
    private float timer = 0.0f;
    CompParticleSystem particles;
    // Use this for initialization
    void Start()
    {
        timer = 0.0f;
        particles = GetComponent<CompParticleSystem>();

        if(particles != null)
        {
            particles.ActivateEmission(true);
        }
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;
        if (timer >= duration)
        {
            Destroy(gameObject);
        }
    }
}