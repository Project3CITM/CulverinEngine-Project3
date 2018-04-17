using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class EnemiesManager : CulverinBehaviour
{
    public List<GameObject> sword_enemies = null;
    public List<GameObject> lance_enemies = null;
    public List<GameObject> shield_enemies = null;
    public GameObject boss = null;

    public void Start()
    {
        sword_enemies = new List<GameObject>();
        lance_enemies = new List<GameObject>();
        shield_enemies = new List<GameObject>();
    }

    public void AddSwordEnemy(GameObject enemy)
    {
        if (sword_enemies == null)
        {
            Debug.Log("[error] EnemiesManager: Sword_enemies is null");
        }
        else
            sword_enemies.Add(enemy);
    }

    public void DeleteSwordEnemy(GameObject enemy)
    {
        if (sword_enemies == null)
        {
            Debug.Log("[error] EnemiesManager: Sword_enemies is null");
        }
        else
        {
            sword_enemies.Remove(enemy);
            EnemySword_BT temp_bt = enemy.GetComponent<EnemySword_BT>();
            temp_bt.DeactivateHUD(temp_bt.sword_icon, temp_bt.sword_name);
        }
    }

    public void AddLanceEnemy(GameObject enemy)
    {
        if (lance_enemies == null)
        {
            Debug.Log("[error] EnemiesManager: Lance_enemies is null");
        }
        else
            lance_enemies.Add(enemy);
    }

    public void DeleteLanceEnemy(GameObject enemy)
    {
        if (lance_enemies == null)
            Debug.Log("[error] EnemiesManager: Lance_enemies is null");
        else
        {
            lance_enemies.Remove(enemy);
            EnemySpear_BT temp_bt = enemy.GetComponent<EnemySpear_BT>();
            temp_bt.DeactivateHUD(temp_bt.spear_icon, temp_bt.spear_name);
        }
    }

    public void AddShieldEnemy(GameObject enemy)
    {
        if (shield_enemies == null)
            Debug.Log("[error] EnemiesManager: Shield_enemies is null");
        else
            shield_enemies.Add(enemy);
    }

    public void DeleteShieldEnemy(GameObject enemy)
    {
        if (shield_enemies == null)
            Debug.Log("[error] EnemiesManager: Shield_enemies is null");
        else
        {
            shield_enemies.Remove(enemy);
            EnemyShield_BT temp_bt = enemy.GetComponent<EnemyShield_BT>();
            temp_bt.DeactivateHUD(temp_bt.shield_icon, temp_bt.shield_name);
        }
    }

    public void AddBoss(GameObject boss_)
    {
        if (boss_ == null)
            Debug.Log("[error] EnemiesManager: Boss_Object is null");
        else
            boss = boss_;
    }

    public void DeleteBoss()
    {
        if (boss == null)
            Debug.Log("[error] EnemiesManager: Boss_Object is null");
        else
            boss = null;
    }

    public BT FindEnemyByTile(int x, int y)
    {
        int size = sword_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            GameObject enemy_t = sword_enemies[k];
            if (enemy_t != null)
            {
                Movement_Action temp_move = enemy_t.GetComponent<Movement_Action>();
                if (temp_move != null) {
                    int tile_x = temp_move.GetCurrentTileX();
                    int tile_y = temp_move.GetCurrentTileY();
                    if (x == tile_x && y == tile_y)
                    {
                        return enemy_t.GetComponent<EnemySword_BT>();
                    }
                }
                else
                    Debug.Log("[error] No movement action!");
            }
            else
                Debug.Log("[error] Sword enemy null!");
        }

        size = lance_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            GameObject enemy_t = lance_enemies[k];
            if (enemy_t != null)
            {
                Movement_Action temp_move = enemy_t.GetComponent<Movement_Action>();
                if (temp_move != null)
                {
                    int tile_x = temp_move.GetCurrentTileX();
                    int tile_y = temp_move.GetCurrentTileY();
                    if (x == tile_x && y == tile_y)
                    {
                        return enemy_t.GetComponent<EnemySpear_BT>();
                    }
                }
                else
                    Debug.Log("[error] No movement action!");
            }
            else
                Debug.Log("[error] Sowrd enemy null!");
        }

        size = shield_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            GameObject enemy_t = shield_enemies[k];
            if (enemy_t != null)
            {
                Movement_Action temp_move = enemy_t.GetComponent<Movement_Action>();
                if (temp_move != null)
                {
                    int tile_x = temp_move.GetCurrentTileX();
                    int tile_y = temp_move.GetCurrentTileY();
                    if (x == tile_x && y == tile_y)
                    {
                        return enemy_t.GetComponent<EnemyShield_BT>();
                    }
                }
                else
                    Debug.Log("[error] No movement action!");
            }
            else
                Debug.Log("[error] Sword enemy null!");
        }

        if (boss != null)
        {
            Movement_Action t_move = boss.GetComponent<Movement_Action>();
            if (t_move != null)
            {
                int tile_x = t_move.GetCurrentTileX();
                int tile_y = t_move.GetCurrentTileY();
                if (x == tile_x && y == tile_y)
                {
                    return boss.GetComponent<Boss_BT>();
                }
            }
            else
                Debug.Log("[error] No movement action!");
        }

        return null;
    }

    public void DamageEnemyInTile(int x,int y, float damage)
    {
        int size = sword_enemies.Count;
        int tile_x = 0;
        int tile_y = 0;
        for (int k = 0; k < size; k++)
        {
            tile_x = sword_enemies[k].GetComponent<Movement_Action>().GetCurrentTileX();
            tile_y = sword_enemies[k].GetComponent<Movement_Action>().GetCurrentTileY();
            if (x == tile_x && y == tile_y)
            {
                sword_enemies[k].GetComponent<EnemySword_BT>().ApplyDamage(damage);
                return;
            }
        }

        size = lance_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            tile_x = lance_enemies[k].GetComponent<Movement_Action>().GetCurrentTileX();
            tile_y = lance_enemies[k].GetComponent<Movement_Action>().GetCurrentTileY();
            if (x == tile_x && y == tile_y)
            {
                lance_enemies[k].GetComponent<EnemySpear_BT>().ApplyDamage(damage);
                return;
            }
        }

        size = shield_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            tile_x = shield_enemies[k].GetComponent<Movement_Action>().GetCurrentTileX();
            tile_y = shield_enemies[k].GetComponent<Movement_Action>().GetCurrentTileY();
            if (x == tile_x && y == tile_y)
            {
                shield_enemies[k].GetComponent<EnemyShield_BT>().ApplyDamage(damage);
                return;
            }
        }

        if (x == boss.GetComponent<Movement_Action>().GetCurrentTileX() && y == boss.GetComponent<Movement_Action>().GetCurrentTileY())
        {
            boss.GetComponent<Boss_BT>().ApplyDamage(damage);
            return;
        }
    }

    public bool ApplyDamage(GameObject target, float damage)
    {
        if (target.GetComponent<EnemySword_BT>() != null)
            return target.GetComponent<EnemySword_BT>().ApplyDamage(damage);
        else if (target.GetComponent<EnemySpear_BT>() != null)
            return target.GetComponent<EnemySpear_BT>().ApplyDamage(damage);
        else if (target.GetComponent<EnemyShield_BT>() != null)
            return target.GetComponent<EnemyShield_BT>().ApplyDamage(damage);
        else if (target.GetComponent<Boss_BT>() != null)
            return target.GetComponent<Boss_BT>().ApplyDamage(damage);
        return true;
    }

    public bool IsEnemy(GameObject target)
    {
        if(target.GetComponent<EnemySword_BT> () != null ||
            target.GetComponent<EnemyShield_BT>() != null ||
            target.GetComponent<EnemySpear_BT>() != null ||
            target.GetComponent<Boss_BT>() != null)
            return true;
        else
            return false;
    }

    public void Push(GameObject target, Vector3 dir)
    {
        if (target.GetComponent<EnemySword_BT>() != null)
            target.GetComponent<EnemySword_BT>().PushEnemy(dir);
        else if (target.GetComponent<EnemySpear_BT>() != null)
            target.GetComponent<EnemySpear_BT>().PushEnemy(dir);
        else if (target.GetComponent<EnemyShield_BT>() != null)
            target.GetComponent<EnemyShield_BT>().PushEnemy(dir);
    }

}

