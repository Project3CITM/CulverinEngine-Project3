using CulverinEditor;
using CulverinEditor.Debug;

public class Hp : CulverinBehaviour
{
    GameObject hp_obj;
    CompImage hp_bar;
    float max_hp = 1.0f;
    float current_hp = 1.0f;
    float damage = 0.01f;
    bool bleeding = true;

    void Start()
    {
        hp_bar = hp_obj.GetComponent<CompImage>();
    }

    void Update()
    {
        if (bleeding)
        {
            GetDamage(damage);
        }
    }

    void GetDamage(float dmg)
    {
        current_hp -= dmg;
        if(current_hp < 0)
        {
            current_hp = 0;
        }
        hp_bar.FillAmount(current_hp);
    }
}