import java.time.LocalDate;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;

/**
 * Абстрактен базов клас за календарни активности.
 * Служи като родителски клас за всички типове активности (задачи, събития, домашни, изпити).
 * 
 * @author Александър Харалампиев
 * @version 1.0
 */
public abstract class CalendarItem {
    private String title;
    private String description;
    private LocalDate date;
    private LocalTime time;
    private String location;
    private Priority priority;
    private boolean isCompleted;
    
    /**
     * Конструктор за създаване на календарна активност.
     * 
     * @param title Заглавие на активността
     * @param description Описание на активността
     * @param date Дата на активността
     * @param time Час на активността
     * @param location Място на провеждане
     * @param priority Приоритет на активността
     */
    public CalendarItem(String title, String description, LocalDate date, 
                       LocalTime time, String location, Priority priority) {
        this.title = title;
        this.description = description;
        this.date = date;
        this.time = time;
        this.location = location;
        this.priority = priority;
        this.isCompleted = false;
    }
    
    /**
     * Абстрактен метод за връщане типа на активността.
     * Всеки наследяващ клас трябва да имплементира този метод.
     * 
     * @return Типа на активността като текст
     */
    public abstract String getType();
    
    /**
     * Маркира активността като завършена.
     */
    public void markAsCompleted() {
        this.isCompleted = true;
    }
    
    // Getters
    public String getTitle() {
        return title;
    }
    
    public String getDescription() {
        return description;
    }
    
    public LocalDate getDate() {
        return date;
    }
    
    public LocalTime getTime() {
        return time;
    }
    
    public String getLocation() {
        return location;
    }
    
    public Priority getPriority() {
        return priority;
    }
    
    public boolean isCompleted() {
        return isCompleted;
    }
    
    // Setters
    public void setTitle(String title) {
        this.title = title;
    }
    
    public void setDescription(String description) {
        this.description = description;
    }
    
    public void setDate(LocalDate date) {
        this.date = date;
    }
    
    public void setTime(LocalTime time) {
        this.time = time;
    }
    
    public void setLocation(String location) {
        this.location = location;
    }
    
    public void setPriority(Priority priority) {
        this.priority = priority;
    }
    
    /**
     * Връща форматирано текстово представяне на активността.
     * 
     * @return Текстово представяне на активността
     */
    @Override
    public String toString() {
        DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("dd.MM.yyyy");
        DateTimeFormatter timeFormatter = DateTimeFormatter.ofPattern("HH:mm");
        
        String status = isCompleted ? "✓" : " ";
        
        return String.format("[%s][%s] %s | %s %s | %s | Приоритет: %s%s",
            status,
            getType(),
            title,
            date.format(dateFormatter),
            time.format(timeFormatter),
            location,
            priority.getDisplayName(),
            description.isEmpty() ? "" : " | " + description
        );
    }
}
