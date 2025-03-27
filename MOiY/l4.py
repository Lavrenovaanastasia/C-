import numpy as np

def clean_matrix(matrix): 
    cleaned = matrix.copy()
    cleaned[np.isclose(cleaned, 0.0)] = 0.0
    return cleaned

def dual_simplex_method(c, A, b, B_initial):
    m, n = A.shape
    B = [idx - 1 for idx in B_initial]
    iteration = 1

    while True:
        print(f"-------------Итерация {iteration}-----------")       
        A_B = A[:, B]
        try:
            A_B_inv = np.linalg.inv(A_B)
        except np.linalg.LinAlgError:
            print("Ошибка: базисная матрица вырождена, метод не может быть применён.")
            return None
        print("Шаг 1. Вычисляем базисную матрицу A_B и её обратную:")
        print("Базисные индексы B:", [i + 1 for i in B])
        print("A_B =", clean_matrix(A_B))
        print("A_B^(-1) =", clean_matrix(A_B_inv))
        c_B = c[B]
        y = A_B_inv.T @ c_B
        print("Шаг 2. Сформируем вектор C_B(компонент вектра + базисные инедексы):")   
        print("c_B =", c_B)
        print("Шаг 3. Вычисляем план двойственной задачи y:")        
        print("y^T = c_B^T * A_B^(-1) =", y)
        kappa_B = A_B_inv @ b
        kappa = np.zeros(n)
        for i, bi in enumerate(B):
            kappa[bi] = kappa_B[i]
        print("Шаг 4. Вычисляем псевдоплан κ:")
        print("κ_B = A_B^(-1) * b =", kappa_B)#компоненты, не входящие в базис, равны 0.
        print("Шаг 5. Псевдоплан κ (все компоненты):\n", kappa)
        if np.all(kappa >= 0):
            print("Шаг 5. все компоненты псевдоплана неотрицательны. Оптимальный план найден.")
            return kappa  
        else:           
            negative_indices = [i for i in range(m) if kappa_B[i] < 0]
            if not negative_indices:
                print( "Ошибка: отрицательная компонента не найдена, хотя условие не выполнено.")
                return None
            k_index = negative_indices[0]
            j_k = B[k_index]
            print(f"Шаг 6. Выбираем отрицательную компоненту псевдоплана: κ[{j_k + 1}] = {kappa[j_k]}")
            delta_y = A_B_inv[k_index, :]
            print("Шаг 7. Вычисляем вектор Δy (строка A_B^(-1) с индексом k):")
            print("Δy =", delta_y)
            nonbasic_indices = [j for j in range(n) if j not in B]
            mu = {}
            print("Шаг 8. Вычисляем m_j для j не принадлежит B:")
            for j in nonbasic_indices:
                mu_j = delta_y @ A[:, j]
                mu[j] = mu_j
                print(f"m[{j + 1}] = Δy^T * A[_ {j + 1}] = {mu_j}")

            if all(mu[j] >= 0 for j in mu):
                print("\n Для всех j вне базиса m[j] >= 0 -> прямая задача не совместна.")
                return None
            sigma = {}
            print("Шаг 9. Вычисляем Q_j для j с m[j] < 0:")
            for j in nonbasic_indices:
                if mu[j] < 0:
                    numerator = c[j] - (A[:, j] @ y)
                    sigma_j = numerator / mu[j]
                    sigma[j] = sigma_j
                    print(f"Q[{j + 1}] = (c[{j + 1}] - A[:, {j + 1}]^T * y) / m[{j + 1}] = {sigma_j}")

            j0, sigma0 = min(sigma.items(), key=lambda item: item[1])
            print(f"Шаг 10. Выбираем j_0 с минимальным Q: Q_o = {sigma0} при j_0 = {j0 + 1}")
            print(f"Шаг 11. Обновляем базис: заменяем базисный индекс {j_k + 1} на {j0 + 1}.")
            B[k_index] = j0
            print("Новый базис B:", [i + 1 for i in B])
            iteration += 1

def get_input_data():   
    m = int(input("m: "))
    n = int(input("n: "))
    c_input = input("c: ").strip().split()    
    c = np.array([float(num) for num in c_input], dtype=float)
    print(f"A:")
    A = []
    for i in range(m):
        row_input = input(f"Строка {i + 1}: ").strip().split()       
        A.append([float(num) for num in row_input])
    A = np.array(A, dtype=float)
    b_input = input("b: ").strip().split()  
    b = np.array([float(num) for num in b_input], dtype=float)
    B_input = input("Введите начальный базис B: ").strip().split()   
    B_initial = [int(idx) for idx in B_input]
    if any(j < 1 or j > n for j in B_initial):
        print("Ошибка: базисные индексы должны быть в диапазоне от 1 до n.")
        exit(1)
    return c, A, b, B_initial

def main():
    c, A, b, B_initial = get_input_data()
    optimal_plan = dual_simplex_method(c, A, b, B_initial)
    if optimal_plan is not None:
        print("Ответ: κ^T = (", end="")
        print(", ".join([f"{val:.2f}" for val in optimal_plan]), end="")
        print(")")
    else:
        print("Прямая задача не совместна (нет допустимого плана).")

if __name__ == "__main__":
    main()