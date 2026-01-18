/**
 * Enum представляващ нивата на приоритет за календарни активности.
 * 
 * @author Александър Харалампиев
 * @version 1.0
 */
public enum Priority {
    /**
     * Нисък приоритет
     */
    LOW("Нисък"),
    
    /**
     * Среден приоритет
     */
    MEDIUM("Среден"),
    
    /**
     * Висок приоритет
     */
    HIGH("Висок");
    
    private final String displayName;
    
    /**
     * Конструктор за Priority enum.
     * 
     * @param displayName Име за показване на потребителя
     */
    Priority(String displayName) {
        this.displayName = displayName;
    }
    
    /**
     * Връща текстово представяне на приоритета.
     * 
     * @return Име на приоритета
     */
    public String getDisplayName() {
        return displayName;
    }
}
