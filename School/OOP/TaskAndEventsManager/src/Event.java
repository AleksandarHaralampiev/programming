import java.time.LocalDate;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;

/**
 * Клас представляващ събитие в календара.
 * Наследява CalendarItem и добавя допълнителни атрибути специфични за събития.
 *
 * @author Александър Харалампиев
 * @version 1.0
 */
public class Event extends CalendarItem {
    private LocalTime endTime;
    private ArrayList<String> participants;

    /**
     * Конструктор за създаване на събитие.
     *
     * @param title Заглавие на събитието
     * @param description Описание на събитието
     * @param date Дата на събитието
     * @param time Начален час на събитието
     * @param location Място на провеждане
     * @param priority Приоритет на събитието
     * @param endTime Краен час на събитието
     */
    public Event(String title, String description, LocalDate date, LocalTime time,
                 String location, Priority priority, LocalTime endTime) {
        super(title, description, date, time, location, priority);
        this.endTime = endTime;
        this.participants = new ArrayList<>();
    }

    /**
     * Връща типа на активността.
     *
     * @return "СЪБИТИЕ"
     */
    @Override
    public String getType() {
        return "СЪБИТИЕ";
    }

    /**
     * Добавя участник към събитието.
     *
     * @param participant Име на участника
     */
    public void addParticipant(String participant) {
        if (!participants.contains(participant)) {
            participants.add(participant);
        }
    }

    /**
     * Премахва участник от събитието.
     *
     * @param participant Име на участника
     */
    public void removeParticipant(String participant) {
        participants.remove(participant);
    }

    // Getters
    public LocalTime getEndTime() {
        return endTime;
    }

    public ArrayList<String> getParticipants() {
        return new ArrayList<>(participants);
    }

    // Setters
    public void setEndTime(LocalTime endTime) {
        this.endTime = endTime;
    }

    /**
     * Връща форматирано текстово представяне на събитието.
     *
     * @return Текстово представяне на събитието
     */
    @Override
    public String toString() {
        DateTimeFormatter timeFormatter = DateTimeFormatter.ofPattern("HH:mm");
        String participantsStr = participants.isEmpty() ?
            "Няма участници" :
            String.join(", ", participants);

        return super.toString() +
               String.format(" | Край: %s | Участници: %s",
                           endTime.format(timeFormatter), participantsStr);
    }
}
