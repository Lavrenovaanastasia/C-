import numpy as np

def input_matrix(m, n):
    matrix = []
    for i in range(m):
        row = list(map(int, input(f"Введите строку {i + 1} (через пробел): ").split()))
        matrix.append(row)
    return np.array(matrix)

def input_vector(size, name):
    return np.array(list(map(int, input(f"Введите вектор {name} размерности {size} (через пробел): ").split())))

def transportation_problem(cost_matrix, supply, demand, balanced_matrix):
    m, n = cost_matrix.shape
    allocation = np.zeros((m, n))
    
    supply = supply.copy()
    demand = demand.copy()

    print("\nЭтап I. Поиск первого опорного плана.")
    
    while np.any(supply) and np.any(demand):
        temp_cost_matrix = np.copy(cost_matrix)
        temp_cost_matrix[supply == 0] = np.inf
        temp_cost_matrix[:, demand == 0] = np.inf

        min_cost_index = np.unravel_index(np.argmin(temp_cost_matrix), temp_cost_matrix.shape)
        i, j = min_cost_index
        
        quantity = min(supply[i], demand[j])
        allocation[i][j] = quantity
        supply[i] -= quantity
        demand[j] -= quantity
        
        print(f"\nИскомый элемент равен c{i+1}{j+1}={cost_matrix[i][j]}. Для этого элемента запасы равны {supply[i] + quantity}, потребности {demand[j] + quantity}.")
        print(f"x{i+1}{j+1} = min({supply[i] + quantity}, {demand[j] + quantity}) = {quantity}.")

        if supply[i] == 0:
            temp_cost_matrix[i, :] = np.inf
        if demand[j] == 0:
            temp_cost_matrix[:, j] = np.inf

        print("\n Текущая таблица распределения:")
        print_allocation(allocation, supply, demand, balanced_matrix)

    return allocation

def print_allocation(allocation, supply, demand, balanced_matrix):
    m, n = allocation.shape
    print("\t" + "\t".join([f"B{i + 1}" for i in range(n)]) + "\tЗапасы")
    
    for i in range(m):
        row = []
        for j in range(n):
            value = allocation[i][j]
            balanced_value = balanced_matrix[i][j]
            if value > 0:
                row.append(f"{int(balanced_value)}[{int(value)}]")  # Значения из сбалансированной матрицы и текущее значение
            else:
                row.append(f"{int(balanced_value)}[0]")  # Значение из сбалансированной матрицы с нулем в скобках
        print(f"A{i + 1}\t" + "\t".join(row) + f"\t{int(supply[i])}")

    print(f"Потребности\t" + "\t".join([f"{int(demand[j])}" for j in range(n)]) + "\t-")

def check_balance(supply, demand):
    total_supply = sum(supply)
    total_demand = sum(demand)

    print(f"\n Проверка на условие баланса:")
    print(f"∑a = {total_supply}")
    print(f"∑b = {total_demand}")
    
    return total_supply, total_demand

def balance_problem(supply, demand, cost_matrix):
    total_supply = sum(supply)
    total_demand = sum(demand)

    print("\n Процесс сбалансировки:")
    if total_supply < total_demand:
        supply = np.append(supply, total_demand - total_supply)
        print(f"Добавлен фиктивный пункт поставки с объемом: {total_demand - total_supply}")
    elif total_supply > total_demand:
        demand = np.append(demand, total_supply - total_demand)
        print(f"Добавлен фиктивный пункт спроса с объемом: {total_supply - total_demand}")

    return supply, demand

def calculate_potentials(allocation, supply, demand, cost_matrix):
    m, n = allocation.shape
    u = np.full(m, np.nan)  
    v = np.full(n, np.nan)  
    u[0] = 0  # Начальное значение u[0]

    print("\nЭтап расчета потенциалов:")
    print(f"Изначальные значения:\nu: {u}\nv: {v}")

    # Итеративный процесс для заполнения u и v
    for iteration in range(m + n):
        print(f"\nИтерация {iteration + 1}:")
        for i in range(m):
            for j in range(n):
                if allocation[i][j] > 0:  # Если ячейка занята
                    if not np.isnan(v[j]):
                        u[i] = cost_matrix[i][j] - v[j]
                        print(f"Обновление u[{i}] = c[{i+1},{j+1}] - v[{j}] => u[{i}] = {cost_matrix[i][j]} - {v[j]} = {u[i]}")
                    elif not np.isnan(u[i]):
                        v[j] = cost_matrix[i][j] - u[i]
                        print(f"Обновление v[{j}] = c[{i+1},{j+1}] - u[{i}] => v[{j}] = {cost_matrix[i][j]} - {u[i]} = {v[j]}")

    # Устанавливаем не определенные u и v в 0
    for i in range(m):
        if np.isnan(u[i]):
            u[i] = 0 

    for j in range(n):
        if np.isnan(v[j]):
            v[j] = 0 

    print(f"\nИтоговые значения:\nu: {u}\nv: {v}")

    # Проверка оптимальности
    optimal = True
    print("\nПроверка оптимальности:")
    for i in range(m):
        for j in range(n):
            if allocation[i][j] > 0:  # Проверяем только занятые клетки
                print(f"Проверка: u[{i}] + v[{j}] ≤ c[{i+1},{j+1}] => {u[i]} + {v[j]} ≤ {cost_matrix[i][j]}")
                if u[i] + v[j] > cost_matrix[i][j]:
                    optimal = False
                    print(f"Нарушение: {u[i]} + {v[j]} > {cost_matrix[i][j]}")

    if optimal:
        print("Опорный план является оптимальным, так все оценки свободных клеток удовлетворяют условию ui + vj ≤ cij.")
    else:
        print("Опорный план не является оптимальным.")

    return u, v

def main():
    m = int(input("Введите количество пунктов отправления (m): "))  
    n = int(input("Введите количество пунктов назначения (n): "))   

    print("Введите матрицу стоимости:")
    cost_matrix = input_matrix(m, n)  

    supply = input_vector(m, "поставок")
    demand = input_vector(n, "запросов")

    total_supply, total_demand = check_balance(supply, demand)

    if total_supply != total_demand:
        supply, demand = balance_problem(supply, demand, cost_matrix)

    balanced_matrix = np.zeros((len(supply), len(demand)))
    balanced_matrix[:m, :n] = cost_matrix  # Копируем из исходной матрицы

    print("\n Сбалансированная матрица:")
    print_allocation(balanced_matrix, supply, demand, balanced_matrix)

    allocation = transportation_problem(balanced_matrix, supply, demand, balanced_matrix)
    
    print("\n Решение (распределение):")
    
    occupied_cells = np.count_nonzero(allocation)
    print_allocation(allocation, supply, demand, balanced_matrix)

    print("\n В результате получен первый опорный план.")
    print(f"Подсчитаем число занятых клеток таблицы, их должно быть m + n - 1 = {m + n - 1}. ")

    objective_value = np.sum(allocation * balanced_matrix)
    print(f"Значение целевой функции для этого опорного плана равно:\nF(x) = {objective_value}")
    
    u, v = calculate_potentials(allocation, supply, demand, balanced_matrix)
    
    print("\n Потенциалы:")
    print("u:", u)
    print("v:", v)

    optimal = True
    for i in range(len(supply)):
        for j in range(len(demand)):
            if allocation[i][j] > 0 and (u[i] + v[j] > balanced_matrix[i][j]):
                optimal = False

    if optimal:
        print("Опорный план является оптимальным.")
    else:
        print("Опорный план не является оптимальным.")

if __name__ == "__main__":
    main()