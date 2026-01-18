import java.time.LocalDate;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.time.format.DateTimeParseException;
import java.util.ArrayList;
import java.util.Scanner;

/**
 * Главен клас на приложението за управление на задачи и събития.
 * Съдържа потребителски интерфейс и логика за управление на календара.
 * 
 * @author Александър Харалампиев
 * @version 1.0
 */
public class TaskManagerApp {
    private static Calendar calendar = new Calendar();
    private static Scanner scanner = new Scanner(System.in);
    private static DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("dd.MM.yyyy");
    private static DateTimeFormatter timeFormatter = DateTimeFormatter.ofPattern("HH:mm");
    
    /**
     * Главен метод на приложението.
     * 
     * @param args Аргументи от командния ред (не се използват)
     */
    public static void main(String[] args) {
        System.out.println("╔═══════════════════════════════════════════════════════════╗");
        System.out.println("║   СИСТЕМА ЗА УПРАВЛЕНИЕ НА ЗАДАЧИ И СЪБИТИЯ              ║");
        System.out.println("║   Personal Task & Event Manager                          ║");
        System.out.println("╚═══════════════════════════════════════════════════════════╝");
        System.out.println();
        
        boolean running = true;
        while (running) {
            running = showMainMenu();
        }
        
        System.out.println("\nБлагодарим, че използвахте приложението! Довиждане!");
        scanner.close();
    }
    
    /**
     * Показва главното меню и обработва избора на потребителя.
     * 
     * @return true за продължаване, false за изход
     */
    private static boolean showMainMenu() {
        System.out.println("\n═══════════════ ГЛАВНО МЕНЮ ═══════════════");
        System.out.println("1. Добави нова активност");
        System.out.println("2. Премахни активност");
        System.out.println("3. Търси активности");
        System.out.println("4. Покажи предстоящи активности");
        System.out.println("5. Покажи завършени активности");
        System.out.println("6. Маркирай активност като завършена");
        System.out.println("7. Преглед на календара (всички активности)");
        System.out.println("8. Изход");
        System.out.println("═══════════════════════════════════════════");
        System.out.print("Изберете опция (1-8): ");
        
        try {
            int choice = Integer.parseInt(scanner.nextLine());
            System.out.println();
            
            switch (choice) {
                case 1:
                    addNewItem();
                    break;
                case 2:
                    removeItem();
                    break;
                case 3:
                    searchItems();
                    break;
                case 4:
                    showUpcomingItems();
                    break;
                case 5:
                    showCompletedItems();
                    break;
                case 6:
                    markAsCompleted();
                    break;
                case 7:
                    viewCalendar();
                    break;
                case 8:
                    return false;
                default:
                    System.out.println("✗ Невалиден избор! Моля изберете число от 1 до 8.");
            }
        } catch (NumberFormatException e) {
            System.out.println("✗ Невалиден вход! Моля въведете число.");
        }
        
        return true;
    }
    
    /**
     * Добавя нова активност в календара.
     */
    private static void addNewItem() {
        System.out.println("──── ДОБАВЯНЕ НА НОВА АКТИВНОСТ ────");
        System.out.println("Изберете тип активност:");
        System.out.println("1. Задача");
        System.out.println("2. Събитие");
        System.out.println("3. Домашно");
        System.out.println("4. Изпит");
        System.out.print("Избор: ");
        
        try {
            int type = Integer.parseInt(scanner.nextLine());
            
            // Общи полета за всички типове
            System.out.print("\nЗаглавие: ");
            String title = scanner.nextLine();
            
            System.out.print("Описание: ");
            String description = scanner.nextLine();
            
            LocalDate date = readDate("Дата (дд.мм.гггг): ");
            LocalTime time = readTime("Час (чч:мм): ");
            
            System.out.print("Локация: ");
            String location = scanner.nextLine();
            
            Priority priority = readPriority();
            
            // Създаване на съответния тип активност
            CalendarItem item = null;
            
            switch (type) {
                case 1:
                    item = createTask(title, description, date, time, location, priority);
                    break;
                case 2:
                    item = createEvent(title, description, date, time, location, priority);
                    break;
                case 3:
                    item = createHomework(title, description, date, time, location, priority);
                    break;
                case 4:
                    item = createExam(title, description, date, time, location, priority);
                    break;
                default:
                    System.out.println("✗ Невалиден тип активност!");
                    return;
            }
            
            if (item != null) {
                calendar.addItem(item);
            }
            
        } catch (NumberFormatException e) {
            System.out.println("✗ Невалиден вход!");
        } catch (Exception e) {
            System.out.println("✗ Грешка: " + e.getMessage());
        }
    }
    
    /**
     * Създава нова задача.
     */
    private static Task createTask(String title, String description, LocalDate date, 
                                   LocalTime time, String location, Priority priority) {
        System.out.print("Очаквана продължителност (минути): ");
        int duration = Integer.parseInt(scanner.nextLine());
        
        System.out.print("Категория (напр. Работа, Лично): ");
        String category = scanner.nextLine();
        
        return new Task(title, description, date, time, location, priority, duration, category);
    }
    
    /**
     * Създава ново събитие.
     */
    private static Event createEvent(String title, String description, LocalDate date, 
                                     LocalTime time, String location, Priority priority) {
        LocalTime endTime = readTime("Краен час (чч:мм): ");
        
        Event event = new Event(title, description, date, time, location, priority, endTime);
        
        System.out.print("Добавете участници? (да/не): ");
        if (scanner.nextLine().equalsIgnoreCase("да")) {
            System.out.println("Въведете имена на участници (празен ред за край):");
            while (true) {
                System.out.print("Участник: ");
                String participant = scanner.nextLine();
                if (participant.isEmpty()) break;
                event.addParticipant(participant);
            }
        }
        
        return event;
    }
    
    /**
     * Създава ново домашно.
     */
    private static Homework createHomework(String title, String description, LocalDate date, 
                                          LocalTime time, String location, Priority priority) {
        System.out.print("Учебен предмет: ");
        String subject = scanner.nextLine();
        
        LocalDate dueDate = readDate("Краен срок (дд.мм.гггг): ");
        
        System.out.print("Зададено от: ");
        String assignedBy = scanner.nextLine();
        
        return new Homework(title, description, date, time, location, priority, 
                          subject, dueDate, assignedBy);
    }
    
    /**
     * Създава нов изпит.
     */
    private static Exam createExam(String title, String description, LocalDate date, 
                                   LocalTime time, String location, Priority priority) {
        System.out.print("Учебен предмет: ");
        String subject = scanner.nextLine();
        
        System.out.print("Продължителност (минути): ");
        int duration = Integer.parseInt(scanner.nextLine());
        
        System.out.print("Зала/Стая: ");
        String room = scanner.nextLine();
        
        return new Exam(title, description, date, time, location, priority, 
                       subject, duration, room);
    }
    
    /**
     * Премахва активност от календара.
     */
    private static void removeItem() {
        System.out.println("──── ПРЕМАХВАНЕ НА АКТИВНОСТ ────");
        
        if (calendar.isEmpty()) {
            System.out.println("Календарът е празен!");
            return;
        }
        
        System.out.print("Въведете заглавие на активността: ");
        String title = scanner.nextLine();
        
        calendar.removeItem(title);
    }
    
    /**
     * Търси активности по различни критерии.
     */
    private static void searchItems() {
        System.out.println("──── ТЪРСЕНЕ НА АКТИВНОСТИ ────");
        System.out.println("1. По дата");
        System.out.println("2. По тип");
        System.out.println("3. По приоритет");
        System.out.print("Изберете критерий: ");
        
        try {
            int choice = Integer.parseInt(scanner.nextLine());
            ArrayList<CalendarItem> results = new ArrayList<>();
            
            switch (choice) {
                case 1:
                    LocalDate date = readDate("Въведете дата (дд.мм.гггг): ");
                    results = calendar.findItemsByDate(date);
                    break;
                case 2:
                    System.out.print("Въведете тип (ЗАДАЧА/СЪБИТИЕ/ДОМАШНО/ИЗПИТ): ");
                    String type = scanner.nextLine();
                    results = calendar.findItemsByType(type);
                    break;
                case 3:
                    Priority priority = readPriority();
                    results = calendar.listItemsByPriority(priority);
                    break;
                default:
                    System.out.println("✗ Невалиден избор!");
                    return;
            }
            
            displayItems(results, "РЕЗУЛТАТИ ОТ ТЪРСЕНЕТО");
            
        } catch (Exception e) {
            System.out.println("✗ Грешка при търсене: " + e.getMessage());
        }
    }
    
    /**
     * Показва предстоящи активности.
     */
    private static void showUpcomingItems() {
        ArrayList<CalendarItem> upcoming = calendar.listUpcomingItems();
        displayItems(upcoming, "ПРЕДСТОЯЩИ АКТИВНОСТИ");
    }
    
    /**
     * Показва завършени активности.
     */
    private static void showCompletedItems() {
        ArrayList<CalendarItem> completed = calendar.listCompletedItems();
        displayItems(completed, "ЗАВЪРШЕНИ АКТИВНОСТИ");
    }
    
    /**
     * Маркира активност като завършена.
     */
    private static void markAsCompleted() {
        System.out.println("──── МАРКИРАНЕ КАТО ЗАВЪРШЕНА ────");
        
        if (calendar.isEmpty()) {
            System.out.println("Календарът е празен!");
            return;
        }
        
        System.out.print("Въведете заглавие на активността: ");
        String title = scanner.nextLine();
        
        calendar.markItemAsCompleted(title);
    }
    
    /**
     * Показва всички активности в календара.
     */
    private static void viewCalendar() {
        calendar.sortByDate();
        ArrayList<CalendarItem> allItems = calendar.getAllItems();
        displayItems(allItems, "КАЛЕНДАР - ВСИЧКИ АКТИВНОСТИ");
    }
    
    /**
     * Показва списък с активности.
     * 
     * @param items Списък с активности
     * @param title Заглавие на списъка
     */
    private static void displayItems(ArrayList<CalendarItem> items, String title) {
        System.out.println("\n═══ " + title + " ═══");
        
        if (items.isEmpty()) {
            System.out.println("Няма намерени активности.");
            return;
        }
        
        System.out.println("Общо: " + items.size() + " активност(и)\n");
        
        for (int i = 0; i < items.size(); i++) {
            System.out.println((i + 1) + ". " + items.get(i));
        }
    }
    
    /**
     * Чете дата от потребителя с валидация.
     * 
     * @param prompt Съобщение за потребителя
     * @return Въведената дата
     */
    private static LocalDate readDate(String prompt) {
        while (true) {
            try {
                System.out.print(prompt);
                String input = scanner.nextLine();
                return LocalDate.parse(input, dateFormatter);
            } catch (DateTimeParseException e) {
                System.out.println("✗ Невалиден формат на дата! Използвайте дд.мм.гггг");
            }
        }
    }
    
    /**
     * Чете час от потребителя с валидация.
     * 
     * @param prompt Съобщение за потребителя
     * @return Въведеният час
     */
    private static LocalTime readTime(String prompt) {
        while (true) {
            try {
                System.out.print(prompt);
                String input = scanner.nextLine();
                return LocalTime.parse(input, timeFormatter);
            } catch (DateTimeParseException e) {
                System.out.println("✗ Невалиден формат на час! Използвайте чч:мм");
            }
        }
    }
    
    /**
     * Чете приоритет от потребителя.
     * 
     * @return Избраният приоритет
     */
    private static Priority readPriority() {
        System.out.println("Приоритет:");
        System.out.println("1. Нисък");
        System.out.println("2. Среден");
        System.out.println("3. Висок");
        
        while (true) {
            try {
                System.out.print("Избор: ");
                int choice = Integer.parseInt(scanner.nextLine());
                
                switch (choice) {
                    case 1:
                        return Priority.LOW;
                    case 2:
                        return Priority.MEDIUM;
                    case 3:
                        return Priority.HIGH;
                    default:
                        System.out.println("✗ Невалиден избор! Изберете 1, 2 или 3.");
                }
            } catch (NumberFormatException e) {
                System.out.println("✗ Невалиден вход! Моля въведете число.");
            }
        }
    }
}
