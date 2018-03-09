using CulverinEditor;
using CulverinEditor.Debug;

public class Shield : CulverinBehaviour
{
    public GameObject shield_obj;
    public GameObject broken_shield_obj;
    CompImage icon;

    public float broken_shield_duration = 2.0f;
    public float shield_duration = 4.0f;
    private float timer = 0;

    private bool active = false;
    private bool broken_shield = false;

    // Use this for initialization
    void Start()
    {
        shield_obj = GetLinkedObject("shield_obj");
        broken_shield_obj = GetLinkedObject("broken_shield_obj");

        // Disable UI images
        icon = shield_obj.GetComponent<CompImage>();
        icon.SetEnabled(false, shield_obj);
        icon = broken_shield_obj.GetComponent<CompImage>();
        icon.SetEnabled(false, broken_shield_obj);
    }

    // Update is called once per frame
    void Update()
    {
        if (active)
        {
            timer += Time.DeltaTime();
            if (timer >= shield_duration)
            {
                //If the time reaches the limit, disable UI.
                DisableShield();
                active = false;
            }
        }

        else if(broken_shield)
        {
            timer += Time.DeltaTime();
            if (timer >= broken_shield_duration)
            {
                //If the time reaches the limit, disable UI.
                DisableBrokenShield();
                broken_shield = false;
            }
        }
    }

    public void ActivateShield()
    {
        EnableShield();

        active = true;
        timer = 0.0f;
    }

    //Change UI from shield to broken and disable the "logic" shield
    public void Break()
    {
        DisableShield();
        EnableBrokenShield();

        active = false;
        broken_shield = true;
        timer = 0.0f;
    }

    //Enable UI Image
    void EnableShield()
    {
        icon = broken_shield_obj.GetComponent<CompImage>();
        icon.SetEnabled(true, shield_obj);
    }

    //Enable UI Image
    void EnableBrokenShield()
    {
        icon = broken_shield_obj.GetComponent<CompImage>();
        icon.SetEnabled(true, broken_shield_obj);
    }

    //Disable UI Image
    void DisableShield()
    {
        icon = broken_shield_obj.GetComponent<CompImage>();
        icon.SetEnabled(false, shield_obj);
    }

    //Disable UI Image
    void DisableBrokenShield()
    {
        icon = broken_shield_obj.GetComponent<CompImage>();
        icon.SetEnabled(false, broken_shield_obj);
    }

    public bool IsActive()
    {
        return active;
    }
}
