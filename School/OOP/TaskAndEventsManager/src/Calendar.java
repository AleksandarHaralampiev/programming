import java.time.LocalDate;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.stream.Collectors;

/**
 * Клас за управление на календар с активности.
 * Съдържа списък от календарни активности и методи за работа с тях.
 * 
 * @author Александър Харалампиев
 * @version 1.0
 */
public class Calendar {
    private ArrayList<CalendarItem> items;
    
    /**
     * Конструктор за създаване на календар.
     */
    public Calendar() {
        this.items = new ArrayList<>();
    }
    
    /**
     * Добавя нова активност в календара.
     * 
     * @param item Календарна активност
     */
    public void addItem(CalendarItem item) {
        items.add(item);
        System.out.println("✓ Активността е добавена успешно!");
    }
    
    /**
     * Премахва активност от календара по заглавие.
     * 
     * @param title Заглавие на активността
     * @return true ако активността е премахната, иначе false
     */
    public boolean removeItem(String title) {
        for (int i = 0; i < items.size(); i++) {
            if (items.get(i).getTitle().equalsIgnoreCase(title)) {
                items.remove(i);
                System.out.println("✓ Активността е премахната успешно!");
                return true;
            }
        }
        System.out.println("✗ Активност с това заглавие не е намерена.");
        return false;
    }
    
    /**
     * Намира всички активности на дадена дата.
     * 
     * @param date Дата за търсене
     * @return Списък с активности на тази дата
     */
    public ArrayList<CalendarItem> findItemsByDate(LocalDate date) {
        ArrayList<CalendarItem> result = new ArrayList<>();
        for (CalendarItem item : items) {
            if (item.getDate().equals(date)) {
                result.add(item);
            }
        }
        return result;
    }
    
    /**
     * Намира всички активности от даден тип.
     * 
     * @param type Тип на активността
     * @return Списък с активности от този тип
     */
    public ArrayList<CalendarItem> findItemsByType(String type) {
        ArrayList<CalendarItem> result = new ArrayList<>();
        for (CalendarItem item : items) {
            if (item.getType().equalsIgnoreCase(type)) {
                result.add(item);
            }
        }
        return result;
    }
    
    /**
     * Връща списък с всички предстоящи активности (незавършени).
     * 
     * @return Списък с предстоящи активности
     */
    public ArrayList<CalendarItem> listUpcomingItems() {
        ArrayList<CalendarItem> upcoming = new ArrayList<>();
        LocalDate today = LocalDate.now();
        
        for (CalendarItem item : items) {
            if (!item.isCompleted() && 
                (item.getDate().isEqual(today) || item.getDate().isAfter(today))) {
                upcoming.add(item);
            }
        }
        
        // Сортираме по дата
        upcoming.sort(Comparator.comparing(CalendarItem::getDate)
                               .thenComparing(CalendarItem::getTime));
        
        return upcoming;
    }
    
    /**
     * Връща списък с всички завършени активности.
     * 
     * @return Списък със завършени активности
     */
    public ArrayList<CalendarItem> listCompletedItems() {
        ArrayList<CalendarItem> completed = new ArrayList<>();
        for (CalendarItem item : items) {
            if (item.isCompleted()) {
                completed.add(item);
            }
        }
        return completed;
    }
    
    /**
     * Сортира всички активности по дата и час.
     */
    public void sortByDate() {
        items.sort(Comparator.comparing(CalendarItem::getDate)
                            .thenComparing(CalendarItem::getTime));
    }
    
    /**
     * Намира всички активности с даден приоритет.
     * 
     * @param priority Приоритет за филтриране
     * @return Списък с активности с този приоритет
     */
    public ArrayList<CalendarItem> listItemsByPriority(Priority priority) {
        ArrayList<CalendarItem> result = new ArrayList<>();
        for (CalendarItem item : items) {
            if (item.getPriority() == priority) {
                result.add(item);
            }
        }
        return result;
    }
    
    /**
     * Маркира активност като завършена по заглавие.
     * 
     * @param title Заглавие на активността
     * @return true ако активността е маркирана, иначе false
     */
    public boolean markItemAsCompleted(String title) {
        for (CalendarItem item : items) {
            if (item.getTitle().equalsIgnoreCase(title)) {
                item.markAsCompleted();
                System.out.println("✓ Активността е маркирана като завършена!");
                return true;
            }
        }
        System.out.println("✗ Активност с това заглавие не е намерена.");
        return false;
    }
    
    /**
     * Връща всички активности в календара.
     * 
     * @return Списък с всички активности
     */
    public ArrayList<CalendarItem> getAllItems() {
        return new ArrayList<>(items);
    }
    
    /**
     * Проверява дали календарът е празен.
     * 
     * @return true ако календарът е празен, иначе false
     */
    public boolean isEmpty() {
        return items.isEmpty();
    }
    
    /**
     * Връща броя на активностите в календара.
     * 
     * @return Брой активности
     */
    public int getSize() {
        return items.size();
    }
}
