import java.time.LocalDate;
import java.time.LocalTime;

/**
 * Клас представляващ задача в календара.
 * Наследява CalendarItem и добавя допълнителни атрибути специфични за задачи.
 * 
 * @author Александър Харалампиев
 * @version 1.0
 */
public class Task extends CalendarItem {
    private int estimatedDuration; // в минути
    private String category;
    
    /**
     * Конструктор за създаване на задача.
     * 
     * @param title Заглавие на задачата
     * @param description Описание на задачата
     * @param date Дата на задачата
     * @param time Час на задачата
     * @param location Място на задачата
     * @param priority Приоритет на задачата
     * @param estimatedDuration Очаквана продължителност в минути
     * @param category Категория на задачата
     */
    public Task(String title, String description, LocalDate date, LocalTime time,
                String location, Priority priority, int estimatedDuration, String category) {
        super(title, description, date, time, location, priority);
        this.estimatedDuration = estimatedDuration;
        this.category = category;
    }
    
    /**
     * Връща типа на активността.
     * 
     * @return "ЗАДАЧА"
     */
    @Override
    public String getType() {
        return "ЗАДАЧА";
    }
    
    // Getters
    public int getEstimatedDuration() {
        return estimatedDuration;
    }
    
    public String getCategory() {
        return category;
    }
    
    // Setters
    public void setEstimatedDuration(int estimatedDuration) {
        this.estimatedDuration = estimatedDuration;
    }
    
    public void setCategory(String category) {
        this.category = category;
    }
    
    /**
     * Връща форматирано текстово представяне на задачата.
     * 
     * @return Текстово представяне на задачата
     */
    @Override
    public String toString() {
        return super.toString() + 
               String.format(" | Категория: %s | Продължителност: %d мин", 
                           category, estimatedDuration);
    }
}
