import java.time.LocalDate;
import java.time.LocalTime;

/**
 * Клас представляващ изпит в календара.
 * Наследява CalendarItem и добавя допълнителни атрибути специфични за изпити.
 * 
 * @author Александър Харалампиев
 * @version 1.0
 */
public class Exam extends CalendarItem {
    private String subject;
    private int duration; // в минути
    private String room;
    private Double grade;
    
    /**
     * Конструктор за създаване на изпит.
     * 
     * @param title Заглавие на изпита
     * @param description Описание на изпита
     * @param date Дата на изпита
     * @param time Час на изпита
     * @param location Локация (сграда)
     * @param priority Приоритет
     * @param subject Учебен предмет
     * @param duration Продължителност в минути
     * @param room Зала/стая
     */
    public Exam(String title, String description, LocalDate date, LocalTime time,
                String location, Priority priority, String subject, 
                int duration, String room) {
        super(title, description, date, time, location, priority);
        this.subject = subject;
        this.duration = duration;
        this.room = room;
        this.grade = null;
    }
    
    /**
     * Връща типа на активността.
     * 
     * @return "ИЗПИТ"
     */
    @Override
    public String getType() {
        return "ИЗПИТ";
    }
    
    /**
     * Задава оценка за изпита.
     * 
     * @param grade Оценка (2.00 - 6.00)
     */
    public void setGrade(Double grade) {
        if (grade >= 2.0 && grade <= 6.0) {
            this.grade = grade;
            // Ако изпитът е положен (оценка >= 3.00), маркираме като завършен
            if (grade >= 3.0) {
                markAsCompleted();
            }
        } else {
            throw new IllegalArgumentException("Оценката трябва да е между 2.00 и 6.00");
        }
    }
    
    // Getters
    public String getSubject() {
        return subject;
    }
    
    public int getDuration() {
        return duration;
    }
    
    public String getRoom() {
        return room;
    }
    
    public Double getGrade() {
        return grade;
    }
    
    // Setters
    public void setSubject(String subject) {
        this.subject = subject;
    }
    
    public void setDuration(int duration) {
        this.duration = duration;
    }
    
    public void setRoom(String room) {
        this.room = room;
    }
    
    /**
     * Връща форматирано текстово представяне на изпита.
     * 
     * @return Текстово представяне на изпита
     */
    @Override
    public String toString() {
        String gradeStr = grade != null ? 
            String.format("Оценка: %.2f", grade) : 
            "Няма оценка";
        
        return super.toString() + 
               String.format(" | Предмет: %s | Зала: %s | Продължителност: %d мин | %s", 
                           subject, room, duration, gradeStr);
    }
}
