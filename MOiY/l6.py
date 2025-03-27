import numpy as np
from numpy.linalg import inv
from numpy.linalg import matrix_rank

def get_input_data_qp():

    n = int(input("n: "))
    m = int(input("m: "))
  
    print(f"\n Введите вектор c :")
    c_input = input("c: ").strip().split()
    c = np.array([float(num) for num in c_input], dtype=float)

    print(f"\n Введите матрицу D :")
    D_list = []
    for i in range(n):
        row_input = input(f"Строка {i+1}: ").strip().split()
        D_list.append([float(num) for num in row_input])
    D = np.array(D_list, dtype=float)

    print(f"\n Введите матрицу A :")
    A_list = []
    for i in range(m):
        row_input = input(f"Строка {i+1}: ").strip().split()      
        A_list.append([float(num) for num in row_input])
    A = np.array(A_list, dtype=float)

    print(f"\n Введите начальный план x' :")
    x_input = input("x: ").strip().split()    
    x = np.array([float(num) for num in x_input], dtype=float)

    print("\n Введите опору ограничений Jb:")
    Jb_input = input("Jb: ").strip().split()   
    Jb = [int(num) for num in Jb_input]

    print("\n Введите расширенную опору ограничений Jb* :")
    Jb_ext_input = input("Jb*: ").strip().split()
    Jb_ext = [int(num) for num in Jb_ext_input]

    # Вычисляем вектор правых частей b = A*x
    b = A.dot(x)
    return c, D, A, b, x, Jb, Jb_ext

def qp_method(c, D, A, b, x, Jb, Jb_ext, tol=1e-10, max_iter=10):  
    
    # Проверка условий
    print("\nПроверяем условия из определения правильного опорного плана:")
    print(f"1. Поскольку |Jb| = {len(Jb)} и rank(A) = {matrix_rank(A)}, то |Jb| = rank(A).")
   
    # Проверка невырожденности матрицы A_B
    A_B = A[:, Jb]
    if np.linalg.det(A_B) != 0:
        print("2. Матрица A_B невырожденная ( она является обратимой: определитель не равен нулю, ранг матрицы равен её порядку).")
    else:
        print("2. Матрица A_B вырожденная.")

    # Вычисляем векторы
   
    print("\n3. Проверяем условия для Jb*:")
    print(f"(a) {Jb} ⊆ {Jb_ext}")
    
    # Проверка обратимости матрицы H
    H = np.block([
        [D[np.ix_(Jb_ext, Jb_ext)], A[:, Jb_ext].T],
        [A[:, Jb_ext], np.zeros((A.shape[0], A.shape[0]))]
    ])
    print(f"(c) Матрица H:\n{H}")
    if np.linalg.det(H) != 0:
        print("Матрица H обратима.")
    else:
        print("Матрица H вырожденная.")  
    # Преобразуем базис и расширенную опору в 0‑индексированный вариант:
    Jb_py = [j - 1 for j in Jb]
    Jb_ext_py = [j - 1 for j in Jb_ext]

    print("Вектор правых частей b =", b)

    iter_num = 1
    n = len(x)
    while iter_num <= max_iter:
        print(f"------------------Итерация {iter_num}-------------------")

        print("\nШаг 1 : Находим векторы c(x),u(x) и delta(x)")
        cx = c + D.dot(x)
        print("\nc(x) = c + D*x =", cx)

        A_B = A[:, Jb_py]
        cB = cx[Jb_py]
        u0 = -np.dot(cB, inv(A_B))
        print("\nu'(x) = -c_B'(x) * (A_B)^(-1) =", u0)

        Delta = np.dot(u0, A) + cx
        print("\ndelta'(x) = u'(x)*A + c'(x) =", Delta)
       
        if np.all(Delta >= -tol):
            print("\n Шаг 2 : Оптимальность достигнута (все компоненты delta(x) >= 0). Оптимальный план x* =", x)
            return x
        
        print("\n Шаг3: Выбираем отрицательную компоненту delta(x)")
        j0_candidates = [j for j, d in enumerate(Delta) if d < -tol]
        if not j0_candidates:
            print(" Все компоненты delta(x) неотрицательны. Алгоритм завершён.")
            return x
        j0 = j0_candidates[0]
        print(f"\n Выбран j_0 = {j0+1} со значением delta(x)[j_0] = {Delta[j0]}")

        print("\n Шаг4: Формируем вектор l'")
        # ШАГ 5. Формируем вектор направления l.
        # Составляем блочную матрицу H:
        D_ext = D[np.ix_(Jb_ext_py, Jb_ext_py)]
        A_ext = A[:, Jb_ext_py]
        zero_block = np.zeros((A.shape[0], A.shape[0]))
        H_top = np.hstack((D_ext, A_ext.T))
        H_bottom = np.hstack((A_ext, zero_block))
        H = np.vstack((H_top, H_bottom))
        print("\nМатрица H:\n", H)

        H_inv = inv(H)
        print("\nОбратная матрица H^-1:\n", H_inv)

        # Строим вектор b*:
        b_star_top = D[np.ix_(Jb_ext_py, [j0])].flatten()
        b_star_bottom = A[:, j0]
        b_star = np.concatenate((b_star_top, b_star_bottom))
        print("\nВектор b* (из элементов D и A для j_0 ):", b_star)

        l_ext = -H_inv.dot(b_star)
        print("\nx = -H^(-1) * b* =", l_ext)

        # Формируем полный вектор направления l (размер n).
        l = np.zeros(n)
        for idx, j in enumerate(Jb_ext_py):
            l[j] = l_ext[idx]
        if j0 not in Jb_ext_py:
            l[j0] = 1
        print("\nВектор направления l:", l)

        delta_val = l.dot(D.dot(l))
        print("\nШаг 5: sigma = l'·D·l =", delta_val)

        theta_j0 = abs(Delta[j0]) / delta_val if delta_val > tol else np.inf
        print("\n Q(j_0) = |delta_j0(x)|/sigma", theta_j0, "т.к. сигма больше 0")

        theta_candidates = []
        for j in Jb_ext_py:
            if l[j] < -tol and x[j] > tol:
                theta_j = -x[j] / l[j]
            else:
                theta_j = np.inf
            theta_candidates.append((theta_j, j))
            print(f"Q_j = {j+1} = -x_j/l_j {theta_j}")

        theta_candidates.append((theta_j0, j0))
        theta0, j_star = min(theta_candidates, key=lambda t: t[0])
        print(f"\nMin Q_0 = {theta0} при индексе j* = {j_star+1}")

        if theta0 == np.inf:
            print("Целевая функция не ограничена снизу на множестве допустимых планов.")
            return x
        if theta0 < tol:
            print(
                "Шаг равен 0 – возможна вырожденность или циклическое поведение. Прерывание алгоритма."
            )
            return x

        # Обновляем план: x = x + Q_0 * l.
        x = x + theta0 * l
        print("\nШаг 6 :Новый допустимый план x после обновления(x=x+Q_0*l):\n", x)

        #  Обновляем опорные множества.
        # Если выбранный индекс j* совпадает с j_0 , базис не меняется, а расширенная опора дополняется.
        if j_star == j0:
            if j0 not in Jb_ext_py:
                Jb_ext.append(j0 + 1)
                Jb_ext_py = [j - 1 for j in Jb_ext]
                print(f"\n По первому правилу: Обновляем расширенную опору: добавляем j_0  = {j0+1} в Jb*.")
            else:
                print("\n Опорные множества не меняются (j_0 уже содержится в Jb*).")
        else:
            print("\n Обновление базиса по правилам (2)–(4) .")
            '''
        else:
            # Реализация пунктов 2-4
            if j0 in Jb_py:
                print(f"\nУдаляем индекс j_0 = {j0 + 1} из Jb.")
                Jb_py.remove(j0)
                Jb.remove(j0 + 1)

            if j_star in Jb_ext_py:
                print(f"\nУдаляем индекс j* = {j_star + 1} из Jb*.")
                Jb_ext_py.remove(j_star)
                Jb_ext.remove(j_star + 1)

            Jb.append(j_star + 1)
            print(f"\nДобавляем индекс j* = {j_star + 1} в Jb.")

'''

        print("Новый базис Jb =", Jb)
        print("Новая расширенная опора Jb* =", Jb_ext)

        iter_num += 1

    print("\nМаксимальное число итераций достигнуто.")
    return x

def main():
    c, D, A, b, x, Jb, Jb_ext = get_input_data_qp() 
    qp_method(c, D, A, b, x, Jb, Jb_ext)

if __name__ == "__main__":
    main()