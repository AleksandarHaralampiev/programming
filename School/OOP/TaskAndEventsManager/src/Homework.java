import java.time.LocalDate;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;

/**
 * Клас представляващ домашна работа в календара.
 * Наследява CalendarItem и добавя допълнителни атрибути специфични за домашни.
 * 
 * @author Александър Харалампиев
 * @version 1.0
 */
public class Homework extends CalendarItem {
    private String subject;
    private LocalDate dueDate;
    private String assignedBy;
    
    /**
     * Конструктор за създаване на домашна работа.
     * 
     * @param title Заглавие на домашното
     * @param description Описание на домашното
     * @param date Дата на задаване
     * @param time Час на задаване
     * @param location Място (напр. "Онлайн")
     * @param priority Приоритет
     * @param subject Учебен предмет
     * @param dueDate Краен срок
     * @param assignedBy От кого е зададено
     */
    public Homework(String title, String description, LocalDate date, LocalTime time,
                    String location, Priority priority, String subject, 
                    LocalDate dueDate, String assignedBy) {
        super(title, description, date, time, location, priority);
        this.subject = subject;
        this.dueDate = dueDate;
        this.assignedBy = assignedBy;
    }
    
    /**
     * Връща типа на активността.
     * 
     * @return "ДОМАШНО"
     */
    @Override
    public String getType() {
        return "ДОМАШНО";
    }
    
    /**
     * Проверява дали домашната работа е просрочена.
     * 
     * @return true ако е просрочена, иначе false
     */
    public boolean isOverdue() {
        return LocalDate.now().isAfter(dueDate) && !isCompleted();
    }
    
    // Getters
    public String getSubject() {
        return subject;
    }
    
    public LocalDate getDueDate() {
        return dueDate;
    }
    
    public String getAssignedBy() {
        return assignedBy;
    }
    
    // Setters
    public void setSubject(String subject) {
        this.subject = subject;
    }
    
    public void setDueDate(LocalDate dueDate) {
        this.dueDate = dueDate;
    }
    
    public void setAssignedBy(String assignedBy) {
        this.assignedBy = assignedBy;
    }
    
    /**
     * Връща форматирано текстово представяне на домашната работа.
     * 
     * @return Текстово представяне на домашното
     */
    @Override
    public String toString() {
        DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("dd.MM.yyyy");
        String overdueMarker = isOverdue() ? " [ПРОСРОЧЕНО!]" : "";
        
        return super.toString() + 
               String.format(" | Предмет: %s | Краен срок: %s | Зададено от: %s%s", 
                           subject, dueDate.format(dateFormatter), assignedBy, overdueMarker);
    }
}
