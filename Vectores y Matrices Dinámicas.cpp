#include <iostream>
#include <iomanip>
#include <cmath> 

const int NUM_PRODUCTS = 5;
const int NUM_MATERIALS = 4;
const int INITIAL_WEEKS = 4;
const int MAX_RETRIES = 3;

class ProductionSystem {
private:
    float** planning_matrix;
    float* unit_costs;
    float* raw_material_inventory;
    int num_weeks;

    const float REQUIREMENTS_MATRIX[NUM_PRODUCTS][NUM_MATERIALS] = {
        {2.0f, 0.0f, 1.5f, 0.5f},
        {1.0f, 3.0f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.5f, 0.5f},
        {4.0f, 1.0f, 2.0f, 0.0f},
        {0.0f, 5.0f, 0.0f, 1.5f}
    };
    const float MP_ACQUISITION_COSTS[NUM_MATERIALS] = {10.0f, 5.0f, 20.0f, 8.0f}; 

    const std::string PRODUCT_NAMES[NUM_PRODUCTS] = {"Componente A", "Componente B", "Componente C", "Componente D", "Componente E"};
    const std::string MATERIAL_NAMES[NUM_MATERIALS] = {"MP-01 (Resina)", "MP-02 (Chip)", "MP-03 (PCB)", "MP-04 (Cables)"};

    void initialize_data() {
        std::cout << "\nCargando datos iniciales de prueba...\n";

        unit_costs[0] = 12.50f;
        unit_costs[1] = 25.00f;
        unit_costs[2] = 8.75f;
        unit_costs[3] = 30.00f;
        unit_costs[4] = 15.25f;

        raw_material_inventory[0] = 5000.0f; 
        raw_material_inventory[1] = 8000.0f; 
        raw_material_inventory[2] = 3000.0f; 
        raw_material_inventory[3] = 4500.0f; 

        for (int i = 0; i < NUM_PRODUCTS; ++i) {
            for (int j = 0; j < num_weeks; ++j) {
                planning_matrix[i][j] = (float)(1000 + i * 100 + j * 50);
            }
        }
        std::cout << "Inicialización de costos, inventario y plan de 4 semanas completada.\n";
    }

    float get_float_input(const std::string& prompt) {
        float value;
        int attempts = 0;
        while (attempts < MAX_RETRIES) {
            std::cout << prompt;
            if (std::cin >> value) {
                return value;
            } else {
                std::cout << "¡Error! Ingrese un valor numérico válido.\n";
                std::cin.clear(); 
                std::cin.ignore(10000, '\n'); 
                attempts++;
            }
        }
        std::cout << "Demasiados intentos fallidos. Usando 0 como valor predeterminado.\n";
        return 0.0f;
    }

    int get_int_input(const std::string& prompt, int min_val, int max_val) {
        int value;
        int attempts = 0;
        while (attempts < MAX_RETRIES) {
            std::cout << prompt;
            if (std::cin >> value) {
                if (value >= min_val && value <= max_val) {
                    return value;
                } else {
                    std::cout << "¡Error! Ingrese un número entre " << min_val << " y " << max_val << ".\n";
                }
            } else {
                std::cout << "¡Error! Ingrese un número entero válido.\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
            attempts++;
        }
        std::cout << "Demasiados intentos fallidos. Saliendo de la función.\n";
        return -1; 
    }

public:
    ProductionSystem(int initial_weeks) : num_weeks(initial_weeks) {
        unit_costs = new float[NUM_PRODUCTS];
        raw_material_inventory = new float[NUM_MATERIALS];

        planning_matrix = new float*[NUM_PRODUCTS];
        for (int i = 0; i < NUM_PRODUCTS; ++i) {
            planning_matrix[i] = new float[num_weeks];
        }

        std::cout << "Asignación inicial de memoria completada.\n";
        initialize_data();
    }

    ~ProductionSystem() {
        std::cout << "\nLiberando memoria de Matriz y Vectores Dinámicos...\n";

        if (planning_matrix) {
            for (int i = 0; i < NUM_PRODUCTS; ++i) {
                delete[] planning_matrix[i];
            }
            delete[] planning_matrix;
            planning_matrix = nullptr;
        }

        if (unit_costs) {
            delete[] unit_costs;
            unit_costs = nullptr;
        }
        if (raw_material_inventory) {
            delete[] raw_material_inventory;
            raw_material_inventory = nullptr;
        }

        std::cout << "Liberación de memoria completada. Sistema cerrado.\n";
    }

    void display_plan() {
        std::cout << "\n--- Plan de Producción (Productos vs. Semanas) ---\n";
        std::cout << "Semanas Planificadas: " << num_weeks << "\n";

        std::cout << std::setw(20) << std::left << "Producto/Semana";
        for (int j = 0; j < num_weeks; ++j) {
            std::cout << std::setw(8) << "Sem-" + std::to_string(j);
        }
        std::cout << "\n" << std::string(20 + 8 * num_weeks, '-') << "\n";

        for (int i = 0; i < NUM_PRODUCTS; ++i) {
            std::cout << std::setw(20) << std::left << PRODUCT_NAMES[i];
            for (int j = 0; j < num_weeks; ++j) {
                std::cout << std::setw(8) << std::fixed << std::setprecision(0) << planning_matrix[i][j];
            }
            std::cout << "\n";
        }
        std::cout << std::string(20 + 8 * num_weeks, '-') << "\n";

        std::cout << "\n--- Costos Unitarios de Producción ---\n";
        for (int i = 0; i < NUM_PRODUCTS; ++i) {
             std::cout << "- " << PRODUCT_NAMES[i] << ": $" << std::fixed << std::setprecision(2) << unit_costs[i] << "\n";
        }
    }

    void resize_planning_matrix() {
        int new_num_weeks = num_weeks + 1;
        std::cout << "\nAgregando Semana " << num_weeks << "...\n";

        float** new_matrix = new float*[NUM_PRODUCTS];
        for (int i = 0; i < NUM_PRODUCTS; ++i) {
            new_matrix[i] = new float[new_num_weeks];

            for (int j = 0; j < num_weeks; ++j) {
                new_matrix[i][j] = planning_matrix[i][j];
            }

            new_matrix[i][num_weeks] = 0.0f;

            delete[] planning_matrix[i];
        }

        delete[] planning_matrix;

        planning_matrix = new_matrix;
        num_weeks = new_num_weeks;

        std::cout << "Matriz redimensionada exitosamente a " << NUM_PRODUCTS << " productos x " << num_weeks << " semanas.\n";
        std::cout << "La nueva semana es la Semana " << num_weeks - 1 << ".\n";
    }

    void modify_production() {
        std::cout << "\n--- Modificar Plan de Producción ---\n";
        int product_index = get_int_input("Ingrese Producto (0-" + std::to_string(NUM_PRODUCTS - 1) + "): ", 0, NUM_PRODUCTS - 1);
        if (product_index == -1) return;

        int week_index = get_int_input("Ingrese Semana (0-" + std::to_string(num_weeks - 1) + "): ", 0, num_weeks - 1);
        if (week_index == -1) return;

        std::cout << "Producto Seleccionado: " << PRODUCT_NAMES[product_index] << "\n";
        std::cout << "Semana Seleccionada: " << week_index << "\n";

        float quantity = get_float_input("Ingrese la nueva cantidad a producir: ");
        if (quantity < 0) quantity = 0; 

        planning_matrix[product_index][week_index] = quantity;

        std::cout << "\n¡Modificación exitosa!\n";
        std::cout << PRODUCT_NAMES[product_index] << " en Semana " << week_index << " ahora es: " << (int)quantity << " unidades.\n";
    }

    float calculate_material_consumption(int week_index, float* total_consumption) {
        float total_units_produced = 0.0f;

        for (int m = 0; m < NUM_MATERIALS; ++m) {
            total_consumption[m] = 0.0f;
        }

        for (int p = 0; p < NUM_PRODUCTS; ++p) {
            float planned_production = planning_matrix[p][week_index];
            total_units_produced += planned_production;

            for (int m = 0; m < NUM_MATERIALS; ++m) {
                total_consumption[m] += planned_production * REQUIREMENTS_MATRIX[p][m];
            }
        }
        return total_units_produced;
    }

    void calculate_cogs_and_inventory() {
        int week_index = get_int_input("Ingrese Semana para el cálculo de costos (0-" + std::to_string(num_weeks - 1) + "): ", 0, num_weeks - 1);
        if (week_index == -1) return;

        std::cout << "\n--- Reporte de Costos (Semana " << week_index << ") ---\n";
        std::cout << std::fixed << std::setprecision(2);

        float total_cogs = 0.0f;
        float total_units_produced;

        float* consumption = new float[NUM_MATERIALS];
        
        total_units_produced = calculate_material_consumption(week_index, consumption);

        for (int p = 0; p < NUM_PRODUCTS; ++p) {
            total_cogs += planning_matrix[p][week_index] * unit_costs[p];
        }

        std::cout << "Producción Total (Semana " << week_index << "): " << (int)total_units_produced << " unidades.\n";
        std::cout << "Costo Total de Producción (COGS): $" << total_cogs << "\n";

        std::cout << "\n--- Consumo de Materia Prima (MP) ---\n";
        float final_inventory_value = 0.0f;
        float total_raw_material_cost_used = 0.0f;

        for (int m = 0; m < NUM_MATERIALS; ++m) {
            float consumed_qty = consumption[m];
            float initial_qty = raw_material_inventory[m];
            float mp_cost = MP_ACQUISITION_COSTS[m];

            float remaining_qty = fmax(0.0f, initial_qty - consumed_qty);

            final_inventory_value += remaining_qty * mp_cost;

            std::cout << "- " << MATERIAL_NAMES[m] << ": " << consumed_qty << " unidades consumidas (De " << initial_qty << " iniciales).\n";
        }

        std::cout << "\nValor del Inventario Final (M.P. restante): $" << final_inventory_value << "\n";

        delete[] consumption;
    }


    void run_menu() {
        int option;
        std::cout << "\n--- Sistema de Planificación y Costos (COGS) ---\n";
        std::cout << "Inicialización exitosa. Matriz de Planificación: " << NUM_PRODUCTS << " Productos x " << num_weeks << " Semanas.\n";

        do {
            std::cout << "\n--- Menú Principal ---\n";
            std::cout << "1. Ver Plan de Producción y Costos Unitarios\n";
            std::cout << "2. Agregar Nueva Semana (Redimensionar Matriz)\n";
            std::cout << "3. Modificar Producción Planificada\n";
            std::cout << "4. Calcular COGS y Valor de Inventario Final\n";
            std::cout << "5. Salir\n";
            std::cout << "Opción seleccionada: ";

            if (!(std::cin >> option)) {
                std::cout << "Entrada inválida. Por favor, intente de nuevo.\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            switch (option) {
                case 1:
                    display_plan();
                    break;
                case 2:
                    resize_planning_matrix();
                    break;
                case 3:
                    modify_production();
                    break;
                case 4:
                    calculate_cogs_and_inventory();
                    break;
                case 5:
                    std::cout << "Saliendo del sistema...\n";
                    break;
                default:
                    std::cout << "Opción no reconocida. Por favor, ingrese un número del 1 al 5.\n";
                    break;
            }
        } while (option != 5);
    }
};

int main() {
    ProductionSystem system(INITIAL_WEEKS);

    system.run_menu();

    return 0;
}
