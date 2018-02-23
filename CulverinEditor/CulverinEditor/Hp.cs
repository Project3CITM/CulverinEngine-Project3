using CulverinEditor;
using CulverinEditor.Debug;

public class Hp2 : CulverinBehaviour
{
    CompImage hp_bar;
    float max_hp = 1.0f;
    float current_hp = 1.0f;
    float damage = 0.01f;
    bool bleeding = true;

    void Start()
    {
        hp_bar = GetComponent<CompImage>();
    }

    void Update()
    {

    }

    public void GetDamage(float dmg)
    {
        current_hp -= dmg;

        if(current_hp < 0)
        {
            current_hp = 0;
        }

        hp_bar.FillAmount(current_hp);
    }

    public float GetCurrentHealth()
    {
        return current_hp;
    }
}