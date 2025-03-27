import numpy as np

def clean_matrix(matrix):
    cleaned = matrix.copy()
    cleaned[np.isclose(cleaned, 0.0)] = 0.0
    return cleaned

def print_iteration(
    iteration, AB, AB_inv, cB, u, delta, j0=None, z=None, theta=None, theta0=None, k=None, j_star=None, B_new=None, x_new=None
):
    print(f"-----------------Итерация {iteration}-------------")
    print("Шаг 1. Строим базисную матрицу AB и находим её обратную:")
    print("AB:")
    print(clean_matrix(AB))
    print("(AB)^(-1):")
    print(clean_matrix(AB_inv))
    
    print("Шаг 2. Формируем вектор cB:")
    print(f" cB: {cB}")
    
    print("Шаг 3. Находим вектор потенциалов:")
    print(f" u: {u}")
    
    print("Шаг 4. Находим вектор оценок:")
    print(f" Δ: {delta}")
 
    if j0 is not None:
        print("Шаг 6. Выбираем входящую переменную:")
        print(f"j_0: {j0 + 1}")
    if z is not None:
        print("Шаг 7. Вычисляем вектор z:")
        print(f" z: {z}")
    if theta is not None:
        print("Шаг 8. Вычисляем вектор Q:")
        print(f" Q: {theta}")
    if theta0 is not None:
        print("Шаг 9. Определяем минимальное Q_0:")
        print(f"Q_0 = {theta0}")
    if k is not None and j_star is not None:
        print("Шаг 11. Выбираем покидающую переменную:")
        print(f"k: {k + 1}")
        print(f"j* = {j_star + 1}")
    if B_new is not None:
        print("Шаг 12. Обновляем базис:")
        B_new_int = [int(b + 1) for b in B_new]
        print(f"B: {B_new_int}")
    if x_new is not None:
        print("Шаг 13. Обновляем план x:")
        print(f"x: {x_new}")

def simplex_main_phase(c, A, x_initial, B_initial):  
    c = np.array(c, dtype=float)
    A = np.array(A, dtype=float)
    x = np.array(x_initial, dtype=float)
    
    # Преобразуем индексы базисных переменных в нулевой индекс (уменьшаем на 1)
    B = [b_idx - 1 for b_idx in B_initial]
 
    m, n = A.shape     
    iteration = 1     
    # Выбираем базисные столбцы из A
    AB = A[:, B]
    
    # Определяем вектор базисных переменных
    x_B = x[B]
    
    # Вычисляем вектор правой части системы ограничений
    b = AB @ x_B  
    
    while True:  
        AB = A[:, B]  # Обновляем 
        
        try:
            AB_inv = np.linalg.inv(AB)  #обратную матрицу к базисной
        except np.linalg.LinAlgError:
            print("AB необратима. Завершение метода.")
            return None  
        
        cB = c[B]  # Выбираем коэффициенты целевой функции для базисных переменных
        
        u = cB @ AB_inv  # Вектор потенциалов
        
        delta = u @ A - c  # Вычисляем вектор оценок
        
        # Если все элементы delta >= 0, найден оптимальный план
        if np.all(delta >= 0):
            print_iteration(iteration, AB, AB_inv, cB, u, delta, j0=None, z=None, theta=None, theta0=None, k=None, j_star=None, B_new=None, x_new=None)
            return x  # Возвращаем оптимальный вектор решений
        
        # Определяем кандидатные индексы на вхождение в базис (где delta < 0)
        j0_candidates = np.where(delta < 0)[0]
        
        # Если подходящих кандидатов нет, возвращаем текущее решение
        if len(j0_candidates) == 0:
            print_iteration(iteration, AB, AB_inv, cB, u, delta, j0=None, z=None, theta=None, theta0=None, k=None, j_star=None, B_new=None, x_new=None)
            return x
        
        j0 = j0_candidates[0]  # Берем первый кандидатный индекс
        
        Aj0 = A[:, j0]  # Столбец матрицы A, соответствующий j0
        
        z = AB_inv @ Aj0  # Выичсляем вектор A_b(-1) * A_j0
        
        # Вычисляем допустимые шаги (θ)
        theta = np.array([x[B[i]] / z[i] if z[i] > 1e-10 else np.inf for i in range(m)])
        theta0 = np.min(theta)  # Выбираем минимальный положительный шаг
        
        # Если θ0 = inf, целевая функция не ограничена сверху
        if theta0 == np.inf:
            print("Функция не ограничена сверху.")
            return None
        
        k = np.where(theta == theta0)[0][0]  # Индекс удаляемой базисной переменной
        j_star = B[k]  # Переменная, которая покинет базис
        
        B_new = B.copy()  # Копируем старый базис
        B_new[k] = j0  # Заменяем одну переменную в базисе
        
        # Обновляем вектор решений
        x_new = x.copy()
        x_new[j0] = theta0  # Новая переменная входит в базис с максимальным шагом
        for i in range(m):  # Корректируем старые базисные переменные
            if i != k:
                x_new[B[i]] -= theta0 * z[i]
        x_new[j_star] = 0  # Убираем старую переменную из базиса
        
        # Выводим результаты итерации
        print_iteration(iteration, AB, AB_inv, cB, u, delta, j0, z, theta, theta0, k, j_star, B_new, x_new)
        B = B_new  # Обновляем базис
        x = x_new  # Обновляем вектор решений
        iteration += 1  # Увеличиваем счетчик итераций

def get_vector(prompt, length, type_func=float):
    while True:
        try:
            values = input(prompt).strip().split()
            if len(values) != length:
                print(f"Необходимо ввести ровно {length} элементов.")
                continue
            vector = [type_func(val) for val in values]
            return vector
        except ValueError:
            print("Необходимо ввести числовые значения.")


def main():
    n = int(input("n: "))
    m = int(input("m: "))

    print(f"Введите вектор c:")
    c = get_vector("c: ", n)

    print(f"Введите матрицу A:")
    A = []
    for i in range(1, m + 1):
        row = get_vector(f"A[{i}]: ", n)
        A.append(row)

    print(f"Введите начальный допустимый план x:")
    x_initial = get_vector("x: ", n)

    print(f"Введите начальный базис B:")
    while True:
        B_initial = get_vector("B: ", m, int)
        if all(1 <= idx <= n for idx in B_initial) and len(set(B_initial)) == m:
            break
        else:
            print("Индексы должны быть уникальными и в диапазоне от 1 до n.")

    optimal_x = simplex_main_phase(c, A, x_initial, B_initial)

    if optimal_x is not None:
        if np.all(np.mod(optimal_x, 1) == 0):
            optimal_x = optimal_x.astype(int)
        print("Оптимальный план:")
        print(optimal_x)
    else:  
        print("Оптимальный план отсутствует")

if __name__ == "__main__":
    main()