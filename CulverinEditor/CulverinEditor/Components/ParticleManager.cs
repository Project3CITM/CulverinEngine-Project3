using CulverinEditor;
using CulverinEditor.Debug;

public class ParticleManager : CulverinBehaviour
{
    public float duration = 5f;
    private float timer = 0.0f;
    // Use this for initialization
    void Start()
    {
        timer = 0.0f;
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