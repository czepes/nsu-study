/*
 * @czepes 24204 Агапченко В.В.
 *
 * Алгоритм Сузуки-Касами (взаимное исключение с токеном), N = 3 узла.
 *
 * Каждый узел = 2 процесса:
 *   requester(i) - основной цикл: запрос -> ожидание токена -> КС -> выход
 *   listener(i)  - обработчик входящих REQUEST (всегда готов принять сообщение)
 *
 * Токен: сообщение TOKEN в канале tok[i]; его содержимое (LN[], очередь Q)
 * лежит в глобальных переменных, которые трогает только текущий владелец токена.
 *
 * Номера запросов считаются по модулю M.
 * У узла j RN[k][j] и LN[j] всегда отличаются от RN[j][j] не более чем на 1
 * (проверяется assert в listener), поэтому конечное число состояний
 * не искажает алгоритм, а модель может работать бесконечно.
 */

#define N 3
#define M 3

mtype = { TOKEN };

chan req[N] = [0] of { byte, byte };   // (от кого, номер запроса), рандеву
chan tok[N] = [1] of { mtype };        // токен в единственном экземпляре

typedef Row { byte r[N] };
Row  RN[N];            // RN[i].r[j] - что узел i знает о номере запроса узла j
byte LN[N];            // токен: номер последнего обслуженного запроса узлов
byte Q[N];             // токен: очередь ожидающих
byte qlen;
bool has_tok[N];
bool crit[N];
bool wait[N];          // узел отправил запрос и ждёт токен
byte ncrit;            // число узлов в КС

proctype listener(byte self)
{
    byte j, n;
    do
    :: atomic {
         req[self] ? j, n ->
         if
         :: n != RN[self].r[j] ->
              assert(n == (RN[self].r[j] + 1) % M);   // окно разницы <= 1
              RN[self].r[j] = n
         :: else
         fi;
         // простаивающий владелец токена отдаёт его, если у j есть запрос
         if
         :: has_tok[self] && !crit[self] && RN[self].r[j] == (LN[j] + 1) % M ->
              has_tok[self] = false;
              tok[j] ! TOKEN
         :: else
         fi
       }
    od
}

proctype requester(byte self)
{
    byte j, k, nxt, h;
    bool inq;

    do
    ::  /* --- хочу в КС --- */
        if
        :: atomic { has_tok[self] -> crit[self] = true; ncrit++ }   // токен у меня
        :: atomic { !has_tok[self] ->
                    RN[self].r[self] = (RN[self].r[self] + 1) % M;
                    wait[self] = true;
                    j = 0;                                       // broadcast REQUEST
                    do
                    :: j < N -> if :: j != self -> req[j] ! self, RN[self].r[self] :: else fi; j++
                    :: else -> break
                    od }
           atomic { tok[self] ? TOKEN ->                           // ждём токен
                    has_tok[self] = true; wait[self] = false;
                    crit[self] = true; ncrit++ }
        fi;

        /* --- критическая секция --- */
        assert(ncrit == 1);
        skip;

        /* --- выход из КС --- */
        atomic {
            crit[self] = false; ncrit--;
            LN[self] = RN[self].r[self];
            k = 0;
            do
            :: k < N ->
                 if
                 :: k != self && RN[self].r[k] == (LN[k] + 1) % M ->
                      inq = false; h = 0;
                      do
                      :: h < qlen -> if :: Q[h] == k -> inq = true :: else fi; h++
                      :: else -> break
                      od;
                      if :: !inq -> Q[qlen] = k; qlen++ :: else fi
                 :: else
                 fi;
                 k++
            :: else -> break
            od;
            if
            :: qlen > 0 ->                         // отдаём токен голове очереди
                 nxt = Q[0]; h = 0;
                 do
                 :: h < qlen - 1 -> Q[h] = Q[h+1]; h++
                 :: else -> break
                 od;
                 qlen--; Q[qlen] = 0;
                 has_tok[self] = false;
                 tok[nxt] ! TOKEN
            :: else
            fi
        }
    od
}

init
{
    atomic {
        has_tok[0] = true;                         // токен изначально у узла 0
        run listener(0); run requester(0);
        run listener(1); run requester(1);
        run listener(2); run requester(2)
    }
}

/* ---------- LTL-свойства ---------- */

#define RESP(i)  ([] (wait[i] -> <> crit[i]))
#define RECUR(i) ([] <> crit[i])

// 1. Безопасность: []
ltl p1_mutex    { [] (ncrit <= 1) }

// 2. Живость-ответ: [] -> <> (нужен fairness)
ltl p2_response { RESP(0) && RESP(1) && RESP(2) }

// 3. Повторяемость: [] <> (нужен fairness)
ltl p3_recur    { RECUR(0) && RECUR(1) && RECUR(2) }
